# dmitree

**dmitree** is a fast, minimalist C utility for visualizing directory structures while intelligently grouping numbered files. It's like `tree`, but smarter — especially useful for datasets and file structures with repetitive, sequential filenames.

## 🔍 Features

- 📁 **Clean Tree Structure**: Recursively lists folders and files with indentation.
- 🔢 **Grouped Numbered Files**: Files like `image_100.jpg`, `image_101.jpg`, etc., are grouped into `image_#.jpg (n files)`.
- 📂 **Directories-Only Mode**: Use `-d` to view just the folder hierarchy.
- 📍 **Custom Start Path**: Analyze any directory, not just `.`.

## 🛠️ Compilation

Make sure you have a C compiler installed (`gcc`, `clang`, etc.).

```bash
gcc -o dmitree tree_grouped.c


---- Will compile it later

🛠️ Build & Compile Instructions
markdown
Copy
Edit
## 🛠️ Build & Compile

To build `dmitree`, all you need is a standard C compiler (tested with `gcc` and `clang`):

```bash
gcc -O2 -Wall -o dmitree dmitree.c
Optional flags:

-O2: Enables compiler optimizations.

-Wall: Enables common warnings to catch potential issues.

If you'd like to rename the output binary or place it in /usr/local/bin for global use:


sudo cp dmitree /usr/local/bin/
Now you can run it anywhere with:

bash
Copy
Edit
dmitree
yaml
Copy
Edit

---

### 📦 Releases

```markdown
## 📦 Releases

You can find precompiled binaries or tagged versions under the [Releases](https://github.com/yourusername/dmitree/releases) section once available.

Each release will include:
- ✅ Compiled binaries for Linux/macOS (and WSL support)
- ✅ Source code snapshot
- ✅ Changelog of updates

If you'd like to build from source, follow the [Build & Compile](#️build--compile) section.
🧾 Changelog Template (Optional, but good practice)
markdown
Copy
Edit
## 🧾 Changelog

### v1.0.0 (Initial Release)
- Added recursive directory traversal
- Implemented grouped display of numbered files (e.g. `img_#.jpg (n files)`)
- Added `-d` flag to display directories only
- Custom root path supported

### Planned (v1.1+)
- File extension filters
- Output to file (e.g., `tree.txt`)
- Color-coded terminal output
