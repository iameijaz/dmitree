# Dmitree 🌳

A lightweight directory tree viewer that intelligently groups numbered files for cleaner output.

![Build & Test](https://github.com/iameijaz/dmitree/actions/workflows/build-and-test.yml/badge.svg)
![Multi-platform](https://github.com/iameijaz/dmitree/actions/workflows/multi-platform.yml/badge.svg)

## Overview

Dmitree is a fast C program that displays directory structures in a tree format while automatically grouping numbered files (like `file_1.txt`, `file_2.txt`) into a single entry (`file_#.txt (2 files)`). Perfect for exploring datasets, image collections, or any directory with many sequentially numbered files.

## Features

- 🗂️ **Smart File Grouping**: Automatically groups numbered files by pattern
- 🌲 **Clean Tree Display**: Easy-to-read hierarchical structure  
- ⚡ **Fast Performance**: Written in C for speed
- 🎯 **Directory-Only Mode**: Use `-d` flag to show just folder structure
- 📁 **Flexible Paths**: Specify any starting directory

## Installation

### From Source

```bash
git clone https://github.com/iameijaz/dmitree.git
cd dmitree
gcc -o dmitree dmitree.c
sudo cp dmitree /usr/local/bin/
```

### Using DEB Package

Download the latest `.deb` file from [Releases](https://github.com/iameijaz/dmitree/releases) and install:

```bash
sudo dpkg -i dmitree_1.0.0_amd64.deb
```

## Usage

```bash
dmitree [options] [directory]
```

### Options

- `-d` : Show directories only (no files)
- `-h, --help` : Show help message

### Examples

```bash
# Show current directory with grouped files
dmitree

# Show only directory structure
dmitree -d

# Analyze specific directory
dmitree /path/to/directory

# Show directory structure of a dataset
dmitree -d ./my-dataset
```

## Example Output

**Before (standard ls -R):**
```
./images/cats:
cat_001.jpg  cat_002.jpg  cat_003.jpg  cat_004.jpg
cat_005.jpg  cat_006.jpg  cat_007.jpg  cat_008.jpg
```

**After (Dmitree):**
```
└── images/
  └── cats/
    ├── cat_#.jpg (8 files)
```

## Use Cases

- 📊 **Dataset Exploration**: Quickly understand structure of ML datasets
- 🖼️ **Image Collections**: Clean view of photo directories  
- 📂 **Log File Analysis**: Group numbered log files
- 🔍 **Project Navigation**: Get familiar with new codebases

## Building from Source

### Prerequisites

- GCC compiler
- Standard C library

### Compile

```bash
gcc -o dmitree dmitree.c
```

### Install System-wide

```bash
sudo cp dmitree /usr/local/bin/
```

## Creating DEB Package

See [PACKAGING.md](PACKAGING.md) for detailed instructions on building DEB packages.

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author
Ijaz Ahmed [Verbit] — github.com/iameijaz

## Changelog

### v1.0.0
- Initial release
- Basic tree display with file grouping
- Directory-only mode
- Command-line argument parsing

---

⭐ If you find Dmitree useful, please give it a star on GitHub!

