#!/usr/bin/env python3
"""
Deer Benchmark Report Generator

Reads benchmarks/results/bench_results.json and produces formatted
Markdown tables suitable for pasting into README.md.

Usage:
    python3 benchmarks/report.py [results_path]

Requires only Python 3 stdlib — no pip dependencies.
"""

import json
import os
import sys


def load_results(path: str) -> dict:
    with open(path) as f:
        return json.load(f)


def fmt(val, precision=2):
    """Format a number with the given precision."""
    if isinstance(val, float):
        return f"{val:.{precision}f}"
    return str(val)


def print_fidelity_table(data: list):
    print("### 4.1 Round-Trip Fidelity\n")
    print("Measures reconstruction quality after compress → decompress.")
    print("Lower MSE = better. Higher cosine similarity = better.\n")
    print("| Turns | Level    | MSE        | Cosine Sim | Max Abs Error |")
    print("|------:|----------|------------|------------|---------------|")
    for r in data:
        print(
            f"| {r['turns']:>5} "
            f"| {r['level']:<8} "
            f"| {fmt(r['mse'], 6):<10} "
            f"| {fmt(r['cosine_similarity'], 6):<10} "
            f"| {fmt(r['max_abs_error'], 6):<13} |"
        )
    print()


def print_compression_table(data: list):
    print("### 4.2 Compression Ratio\n")
    print("Compares Deer's KVTC/TurboQuant pipeline against raw gzip baseline.\n")
    print(
        "| Turns | Level    | Raw (B) | Deer (B) | Deer %  "
        "| gzip (B) | gzip %  | Chunks | Features |"
    )
    print(
        "|------:|----------|--------:|---------:|--------:"
        "|---------:|--------:|-------:|---------:|"
    )
    for r in data:
        print(
            f"| {r['turns']:>5} "
            f"| {r['level']:<8} "
            f"| {r['raw_bytes']:>7,} "
            f"| {r['deer_bytes']:>8,} "
            f"| {fmt(r['deer_compression_pct'], 1):>6}% "
            f"| {r['gzip_bytes']:>8,} "
            f"| {fmt(r['gzip_compression_pct'], 1):>6}% "
            f"| {r['chunks']:>6} "
            f"| {r['features']:>8} |"
        )
    print()


def print_latency_table(data: list):
    print("### 4.3 Latency (μs, 100 iterations)\n")
    print("**Compress**\n")
    print("| Turns | Level    | Mean     | P50      | P95      | P99      | Stddev   |")
    print("|------:|----------|----------|----------|----------|----------|----------|")
    for r in data:
        c = r["compress_us"]
        print(
            f"| {r['turns']:>5} "
            f"| {r['level']:<8} "
            f"| {fmt(c['mean'], 1):>8} "
            f"| {fmt(c['p50'], 1):>8} "
            f"| {fmt(c['p95'], 1):>8} "
            f"| {fmt(c['p99'], 1):>8} "
            f"| {fmt(c['stddev'], 1):>8} |"
        )

    print("\n**Decompress**\n")
    print("| Turns | Level    | Mean     | P50      | P95      | P99      | Stddev   |")
    print("|------:|----------|----------|----------|----------|----------|----------|")
    for r in data:
        d = r["decompress_us"]
        print(
            f"| {r['turns']:>5} "
            f"| {r['level']:<8} "
            f"| {fmt(d['mean'], 1):>8} "
            f"| {fmt(d['p50'], 1):>8} "
            f"| {fmt(d['p95'], 1):>8} "
            f"| {fmt(d['p99'], 1):>8} "
            f"| {fmt(d['stddev'], 1):>8} |"
        )
    print()


def print_golden_fact_table(data: list):
    print("### 4.4 Golden Fact Signal Retention\n")
    print(
        "200-turn conversation with 20 embedded facts. Measures whether fact-bearing chunks\n"
        "retain distinguishable feature signatures after compression. Cohen's d > 0.5 indicates\n"
        "medium separation; > 0.8 indicates large separation.\n"
    )
    print(
        "| Level    | Fact Norm (μ±σ)   | Filler Norm (μ±σ)  "
        "| Cohen's d | Detection % |"
    )
    print(
        "|----------|-------------------|--------------------|"
        "-----------|-------------|"
    )
    for r in data:
        fact_str = f"{fmt(r['fact_norm_mean'], 4)}±{fmt(r['fact_norm_std'], 4)}"
        filler_str = f"{fmt(r['filler_norm_mean'], 4)}±{fmt(r['filler_norm_std'], 4)}"
        print(
            f"| {r['level']:<8} "
            f"| {fact_str:<17} "
            f"| {filler_str:<18} "
            f"| {fmt(r['cohens_d'], 4):>9} "
            f"| {fmt(r['detection_rate_pct'], 1):>10}% |"
        )
    print()


def print_summary(results: dict):
    print("### 4.5 Summary\n")

    # Best compression ratio
    comp = results.get("compression", [])
    if comp:
        best = max(comp, key=lambda r: r["deer_compression_pct"])
        print(
            f"- **Best compression**: {fmt(best['deer_compression_pct'], 1)}% "
            f"reduction at {best['turns']} turns, `--{best['level']}` level"
        )

    # Typical latency
    lat = results.get("latency", [])
    if lat:
        balanced_200 = [r for r in lat if r["turns"] == 200 and r["level"] == "balanced"]
        if balanced_200:
            c = balanced_200[0]["compress_us"]
            print(
                f"- **Typical compress latency**: {fmt(c['p50'], 0)} μs "
                f"(balanced, 200 turns, p50)"
            )

    # Fidelity
    fid = results.get("fidelity", [])
    if fid:
        balanced = [r for r in fid if r["level"] == "balanced"]
        if balanced:
            avg_cosine = sum(r["cosine_similarity"] for r in balanced) / len(balanced)
            print(f"- **Average cosine similarity** (balanced): {fmt(avg_cosine, 4)}")

    # Golden fact
    gf = results.get("golden_fact", [])
    if gf:
        best_det = max(gf, key=lambda r: r["detection_rate_pct"])
        print(
            f"- **Best fact detection**: {fmt(best_det['detection_rate_pct'], 1)}% "
            f"at `--{best_det['level']}` level "
            f"(Cohen's d = {fmt(best_det['cohens_d'], 3)})"
        )

    print()


def main():
    default_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "results", "bench_results.json"
    )
    path = sys.argv[1] if len(sys.argv) > 1 else default_path

    if not os.path.exists(path):
        print(f"Error: Results file not found at {path}", file=sys.stderr)
        print("Run ./build/deer_bench first to generate results.", file=sys.stderr)
        sys.exit(1)

    results = load_results(path)

    print("## 4. Evaluation Results\n")
    print(
        "All experiments were run with seeded PRNG (seed=42) for reproducibility.\n"
        "Conversation data is synthetically generated to provide controlled conditions.\n"
    )

    if "fidelity" in results:
        print_fidelity_table(results["fidelity"])
    if "compression" in results:
        print_compression_table(results["compression"])
    if "latency" in results:
        print_latency_table(results["latency"])
    if "golden_fact" in results:
        print_golden_fact_table(results["golden_fact"])

    print_summary(results)


if __name__ == "__main__":
    main()
