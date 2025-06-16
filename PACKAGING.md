# Creating a DEB Package for Dmitree

This guide walks you through creating a Debian package (.deb) for Dmitree.

## Prerequisites

Install the required packaging tools:

```bash
sudo apt update
sudo apt install build-essential devscripts debhelper dh-make
```

## Step 1: Prepare the Source

1. Create a clean directory structure:

```bash
mkdir -p dmitree-1.0.0
cd dmitree-1.0.0
```

2. Copy your source files:

```bash
cp /path/to/dmitree.c .
cp /path/to/README.md .
```

3. Create a simple Makefile:

```bash
cat > Makefile << 'EOF'
CC=gcc
CFLAGS=-Wall -O2
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

dmitree: dmitree.c
	$(CC) $(CFLAGS) -o dmitree dmitree.c

install: dmitree
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 dmitree $(DESTDIR)$(BINDIR)

clean:
	rm -f dmitree

.PHONY: install clean
EOF
```

## Step 2: Create Debian Package Structure

1. Initialize the debian directory:

```bash
mkdir debian
```

2. Create `debian/control`:

```bash
cat > debian/control << 'EOF'
Source: dmitree
Section: utils
Priority: optional
Maintainer: Your Name <your.email@example.com>
Build-Depends: debhelper (>= 10), gcc
Standards-Version: 4.1.2
Homepage: https://github.com/yourusername/dmitree

Package: dmitree
Architecture: any
Depends: ${shlibs:Depends}, ${misc:Depends}
Description: Smart directory tree viewer with file grouping
 Dmitree is a lightweight directory tree viewer that intelligently 
 groups numbered files for cleaner output. Perfect for exploring 
 datasets, image collections, or any directory with many 
 sequentially numbered files.
 .
 Features include smart file grouping, clean tree display, fast 
 performance, and directory-only mode.
EOF
```

3. Create `debian/changelog`:

```bash
cat > debian/changelog << 'EOF'
dmitree (1.0.0-1) unstable; urgency=medium

  * Initial release
  * Smart file grouping functionality
  * Directory-only mode
  * Clean tree display

 -- Your Name <your.email@example.com>  Mon, 16 Jun 2025 12:00:00 +0000
EOF
```

4. Create `debian/compat`:

```bash
echo "10" > debian/compat
```

5. Create `debian/rules`:

```bash
cat > debian/rules << 'EOF'
#!/usr/bin/make -f

%:
	dh $@

override_dh_auto_install:
	$(MAKE) DESTDIR=$(CURDIR)/debian/dmitree install
EOF

chmod +x debian/rules
```

6. Create `debian/copyright`:

```bash
cat > debian/copyright << 'EOF'
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: dmitree
Source: https://github.com/yourusername/dmitree

Files: *
Copyright: 2025 Your Name <your.email@example.com>
License: MIT

License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
EOF
```

## Step 3: Build the DEB Package

1. Build the package:

```bash
dpkg-buildpackage -us -uc -b
```

This creates the `.deb` file in the parent directory.

2. Check the package:

```bash
cd ..
ls -la *.deb
```

You should see: `dmitree_1.0.0-1_amd64.deb` (architecture may vary)

## Step 4: Test the Package

1. Install the package:

```bash
sudo dpkg -i dmitree_1.0.0-1_amd64.deb
```

2. Test it works:

```bash
dmitree --help
```

3. Remove if needed:

```bash
sudo dpkg -r dmitree
```

## Step 5: Package Information

Check package details:

```bash
dpkg-deb -I dmitree_1.0.0-1_amd64.deb
dpkg-deb -c dmitree_1.0.0-1_amd64.deb  # List contents
```

## Automated Build Script

Create `build-deb.sh` for easy building:

```bash
cat > build-deb.sh << 'EOF'
#!/bin/bash

VERSION="1.0.0"
ARCH=$(dpkg --print-architecture)

echo "Building Dmitree DEB package v${VERSION}..."

# Clean previous builds
rm -rf dmitree-${VERSION}
rm -f dmitree_${VERSION}*.deb

# Create source directory
mkdir -p dmitree-${VERSION}
cd dmitree-${VERSION}

# Copy source files
cp ../dmitree.c .
cp ../README.md .

# Create Makefile
cat > Makefile << 'MAKEFILE_EOF'
CC=gcc
CFLAGS=-Wall -O2
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin

dmitree: dmitree.c
	$(CC) $(CFLAGS) -o dmitree dmitree.c

install: dmitree
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 dmitree $(DESTDIR)$(BINDIR)

clean:
	rm -f dmitree

.PHONY: install clean
MAKEFILE_EOF

# Copy debian directory
cp -r ../debian .

# Build package
dpkg-buildpackage -us -uc -b

# Move to parent and clean
cd ..
echo "Package built: dmitree_${VERSION}-1_${ARCH}.deb"
rm -rf dmitree-${VERSION}
EOF

chmod +x build-deb.sh
```

## GitHub Actions (Optional)

For automated building, create `.github/workflows/build-deb.yml`:

```yaml
name: Build DEB Package

on:
  push:
    tags:
      - 'v*'

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt update
        sudo apt install build-essential devscripts debhelper
    
    - name: Build DEB package
      run: ./build-deb.sh
    
    - name: Upload DEB as artifact
      uses: actions/upload-artifact@v3
      with:
        name: dmitree-deb
        path: "*.deb"
```

## Troubleshooting

**Common issues:**

1. **Permission denied**: Make sure `debian/rules` is executable
2. **Missing dependencies**: Install `build-essential debhelper`
3. **Architecture mismatch**: The package will be built for your current architecture

**Linting the package:**

```bash
lintian dmitree_1.0.0-1_amd64.deb
```

This checks for common packaging issues.
