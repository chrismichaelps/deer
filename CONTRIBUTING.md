# Contributing to Deer

Contributions are welcome! Here's how you can help:

## Ways to Contribute

- Report bugs and issues
- Suggest new features
- Submit pull requests
- Improve documentation
- Share Deer with others

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/chrismichaelps/deer.git`
3. Create a branch: `git checkout -b feature/your-feature`
4. Make your changes
5. Test with `./build/deer`
6. Commit with clear messages
7. Push and create a PR

## Coding Standards

- Follow existing code style (2-space indentation)
- Add FMCF-style comments: `@deer.module.Function`
- Update hash registry when adding new files
- Test all CLI commands before submitting

## Building

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Testing

```bash
# Test all CLI commands
echo '{"test":1}' | ./build/deer add
./build/deer compress --max
./build/deer decompress
./build/deer resume
```

## Questions

Open an issue for discussion before starting major changes.
