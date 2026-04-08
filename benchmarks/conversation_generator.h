#pragma once
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <vector>

namespace deer::bench {

using json = nlohmann::json;

/**
 * Pool of realistic filler conversation turns.
 * Intentionally varied in length, punctuation density, and keyword presence
 * so the feature extractor sees meaningful variance.
 */
inline const std::vector<std::pair<std::string, std::string>> FILLER_POOL = {
    {"user", "How do distributed consensus algorithms like Raft handle network partitions? "
             "I keep reading conflicting explanations online."},
    {"assistant", "In Raft, when a network partition occurs the cluster splits into groups. "
                  "Only the group containing a majority of nodes can elect a leader and "
                  "continue processing writes. The minority side stalls until the partition "
                  "heals and it can rejoin and replay the log."},
    {"user", "What happens to writes that were accepted on the minority side before "
             "the partition was detected?"},
    {"assistant", "If the minority side had the old leader, any uncommitted entries get "
                  "overwritten when the partition heals. Only entries replicated to a "
                  "majority are considered committed. This is why Raft guarantees "
                  "linearizability — uncommitted writes are never visible to clients."},
    {"user", "I need to set up a PostgreSQL replication topology. Should I use "
             "streaming replication or logical replication?"},
    {"assistant", "Streaming replication sends WAL bytes directly to standbys. It is simpler "
                  "and gives you hot standby reads. Logical replication decodes WAL into "
                  "logical changes, allowing selective table replication and cross-version "
                  "upgrades. Use streaming for HA failover, logical for data distribution."},
    {"user", "We are seeing 99th percentile latency spikes every 30 seconds on our "
             "gRPC service. CPU and memory look flat."},
    {"assistant", "Periodic spikes with flat resource usage often point to garbage collection "
                  "pauses, connection pool exhaustion, or TLS session ticket rotation. Check "
                  "your GC logs first. If you are on Go, look at runtime/trace output for "
                  "stop-the-world events."},
    {"user", "We switched from REST to gRPC and throughput improved but tail latency "
             "got worse. Any ideas?"},
    {"assistant", "gRPC uses HTTP/2 multiplexing which can cause head-of-line blocking at "
                  "the TCP layer. A single retransmission stalls all streams on that "
                  "connection. Consider using multiple connections, enabling retry policies, "
                  "or investigating QUIC-based transports."},
    {"user", "Can you explain the CAP theorem in practical terms? I understand the "
             "theory but not how it applies to real systems."},
    {"assistant", "In practice, network partitions will happen, so you are always choosing "
                  "between consistency and availability during a partition. DynamoDB chooses "
                  "availability (AP), while Spanner chooses consistency (CP) using TrueTime. "
                  "Most systems let you tune this per-operation."},
    {"user", "How should I structure integration tests for a microservice that "
             "depends on Kafka, Redis, and PostgreSQL?"},
    {"assistant", "Use Testcontainers to spin up real instances of each dependency in Docker. "
                  "Keep integration tests in a separate test suite from unit tests so you can "
                  "run them independently. Each test should set up its own topic, key prefix, "
                  "and schema to avoid cross-test contamination."},
    {"user", "What is the difference between blue-green deployments and canary releases? "
             "When would you pick one over the other?"},
    {"assistant", "Blue-green swaps all traffic at once between two identical environments. "
                  "It is simpler but doubles infrastructure cost during deployment. Canary "
                  "gradually shifts a percentage of traffic to the new version while "
                  "monitoring error rates. Use canary when you need fine-grained rollback."},
    {"user", "We are evaluating whether to move from a monolith to microservices. "
             "The team has 8 engineers. Is it worth the overhead?"},
    {"assistant", "For a team of 8, microservices often introduce more operational complexity "
                  "than they solve. You would need service mesh, distributed tracing, and "
                  "contract testing. Consider a modular monolith first — clear module "
                  "boundaries with in-process calls, extractable later if needed."},
    {"user", "Our CI pipeline takes 45 minutes. What are the highest-impact "
             "optimizations we should try first?"},
    {"assistant", "Start with dependency caching — Docker layer cache, npm/pip cache, and "
                  "compiled artifact cache. Then parallelize test suites across multiple "
                  "runners. Finally, consider test impact analysis to only run tests "
                  "affected by the changed files. These three alone often cut time by 60%."},
    {"user", "How does connection pooling work in database drivers? I keep hitting "
             "too many connections errors."},
    {"assistant", "A connection pool maintains a set of reusable database connections. When "
                  "your code requests a connection, the pool hands out an idle one instead "
                  "of opening a new TCP socket. Set max_pool_size to roughly 2-3x your CPU "
                  "cores. The 'too many connections' error means your pool size times your "
                  "service replica count exceeds the database max_connections setting."},
    {"user", "What is the best way to handle secrets in a Kubernetes cluster? "
             "We currently have them in environment variables."},
    {"assistant", "Environment variables are the weakest option since they appear in pod "
                  "specs and process listings. Use Kubernetes Secrets mounted as volumes, "
                  "or better yet, integrate an external secrets manager like HashiCorp "
                  "Vault or AWS Secrets Manager with the CSI driver."},
    {"user", "When should I use an event-driven architecture versus a request-response "
             "pattern? I see teams defaulting to events for everything."},
    {"assistant", "Use events when you need asynchronous decoupling, audit trails, or fan-out "
                  "to multiple consumers. Stick with request-response when you need immediate "
                  "confirmation, transactions, or simple CRUD operations. Over-eventing leads "
                  "to debugging nightmares because causality becomes hard to trace."},
};

/**
 * Default set of golden facts with distinctive textual signatures.
 *
 * These simulate personal preferences, configuration constraints, and project
 * metadata that a memory system should preserve across long conversations.
 * Intentionally varied in feature density (digits, URLs, code tokens, etc.).
 */
inline const std::vector<std::string> DEFAULT_GOLDEN_FACTS = {
    "The test user's preferred language is TypeScript, never plain JavaScript.",
    "The staging database is hosted at jdbc:postgresql://10.0.3.47:5432/staging_v2.",
    "The deployment window is Tuesdays and Thursdays between 14:00-16:00 UTC only.",
    "The project repository is located at https://example.com/git/atlas-engine.git.",
    "The production cluster runs Kubernetes 1.29 on 48 nodes across 3 availability zones.",
    "The team agreed on a strict 80% code coverage minimum enforced by the CI gate.",
    "IMPORTANT: The payment service must never be deployed independently of the auth service.",
    "The Redis cache TTL for session tokens is exactly 3600 seconds (1 hour).",
    "The upstream vendor API has a hard rate limit of 500 requests per minute per API key.",
    "The monitoring stack uses Prometheus 2.51, Grafana 10.3, and Alertmanager 0.27.",
    "TODO: Migrate from MongoDB 5.0 to PostgreSQL 16 before Q3 2026 deadline.",
    "The S3 bucket for backups is s3://acme-atlas-backups-us-east-1/weekly/ with versioning on.",
    "All gRPC services must implement health checks on port 8081 with a /readyz endpoint.",
    "NOTE: The feature flag system uses LaunchDarkly with SDK key sdk-42f8-prod-9a1b.",
    "The load balancer terminates TLS with a wildcard cert for *.atlas.acme-corp.dev.",
    "Maximum allowed request payload size is 10 MB, enforced at the API gateway layer.",
    "The on-call rotation follows a 7-day cycle with handoff at 09:00 America/Chicago.",
    "Critical: the search index rebuild job must complete within the 4-hour maintenance window.",
    "The data retention policy requires purging PII after exactly 730 days (2 years).",
    "The CI pipeline artifact storage quota is 50 GB per project with 90-day expiry.",
};

/**
 * Generates a conversation of filler turns by cycling through the pool.
 *
 * @param num_turns Number of turns to generate.
 * @param seed      PRNG seed for shuffled sampling.
 * @return JSON array of {role, content} turn objects.
 */
inline json generate_filler(size_t num_turns, unsigned int seed = 42) {
    json turns = json::array();
    std::mt19937 rng(seed);

    std::vector<size_t> indices(FILLER_POOL.size());
    std::iota(indices.begin(), indices.end(), 0);

    for (size_t i = 0; i < num_turns; ++i) {
        if (i % FILLER_POOL.size() == 0) {
            std::shuffle(indices.begin(), indices.end(), rng);
        }
        const auto &[role, content] = FILLER_POOL[indices[i % FILLER_POOL.size()]];
        turns.push_back({{"role", role}, {"content", content}});
    }
    return turns;
}

/**
 * Generates a conversation with golden facts embedded at evenly-spaced positions.
 *
 * Facts are injected as user turns so they occupy deterministic chunk positions
 * that can be tracked through compression.
 *
 * @param num_turns  Total number of turns (filler + facts).
 * @param facts      Vector of fact strings to inject.
 * @param seed       PRNG seed for filler generation.
 * @return Pair of (JSON turns array, vector of indices where facts were placed).
 */
inline std::pair<json, std::vector<size_t>>
generate_with_golden_facts(size_t num_turns,
                           const std::vector<std::string> &facts = DEFAULT_GOLDEN_FACTS,
                           unsigned int seed = 42) {
    json turns = generate_filler(num_turns, seed);
    std::vector<size_t> fact_indices;

    if (facts.empty() || num_turns < facts.size()) {
        return {turns, fact_indices};
    }

    const size_t spacing = num_turns / facts.size();
    for (size_t i = 0; i < facts.size(); ++i) {
        const size_t idx = std::min(i * spacing + (spacing / 2), num_turns - 1);
        turns[idx] = {{"role", "user"}, {"content", facts[i]}};
        fact_indices.push_back(idx);
    }

    return {turns, fact_indices};
}

} // namespace deer::bench
