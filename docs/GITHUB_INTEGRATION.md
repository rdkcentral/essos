# GitHub Integration Guide - Essos L1 Test Suite

This guide shows you how to integrate your Essos L1 test suite with GitHub for automated CI/CD.

---

## Table of Contents
1. [Quick Setup (5 Minutes)](#quick-setup-5-minutes)
2. [Step-by-Step Integration](#step-by-step-integration)
3. [What GitHub CI Does](#what-github-ci-does)
4. [Viewing Results](#viewing-results)
5. [Advanced Configuration](#advanced-configuration)
6. [Troubleshooting](#troubleshooting)

---

## Quick Setup (5 Minutes)

### Prerequisites
✅ GitHub account  
✅ Git installed on your machine  
✅ Your Essos project code ready  

### Steps

**1. Create GitHub Repository**
```bash
# Go to GitHub.com and create a new repository
# Name it: essos-l1-tests (or your preferred name)
# Don't initialize with README (we already have files)
```

**2. Initialize Git (if not already done)**
```bash
cd "d:\Westeros\essos-topic-RDK-58448_1 1\essos-topic-RDK-58448_1"

# Initialize git
git init

# Add .gitignore
cat > .gitignore << 'EOF'
# Build artifacts
build/
*.o
*.obj
*.exe
*.out
*.a
*.so
*.dll

# Test results
test_results.xml
coverage.info
coverage_html/
*.gcno
*.gcda
*.gcov

# Analysis results
static_analysis.log
valgrind_report.xml

# IDE files
.vscode/
.vs/
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db

# Temporary files
*.log
*.tmp
EOF
```

**3. Connect to GitHub**
```bash
# Add all files
git add .

# Create initial commit
git commit -m "Initial commit: Essos L1 Test Suite"

# Add GitHub remote (replace with your repository URL)
git remote add origin https://github.com/YOUR_USERNAME/essos-l1-tests.git

# Push to GitHub
git branch -M main
git push -u origin main
```

**4. Verify CI Workflow**
- Go to your GitHub repository
- Click "Actions" tab
- You should see a workflow run starting automatically
- Wait for it to complete (green checkmark = success)

**That's it!** 🎉 Your CI is now running automatically.

---

## Step-by-Step Integration

### 1. Prepare Your Repository

#### A. Create GitHub Repository
1. Go to https://github.com
2. Click the "+" icon → "New repository"
3. Fill in details:
   - **Repository name**: `essos-l1-tests`
   - **Description**: "Essos L1 Test Suite with automated CI/CD"
   - **Visibility**: Public or Private (your choice)
   - **DO NOT** check "Initialize with README"
4. Click "Create repository"

#### B. Save Your Repository URL
```
https://github.com/YOUR_USERNAME/essos-l1-tests.git
```

### 2. Configure Git Locally

```bash
# Navigate to your project
cd "d:\Westeros\essos-topic-RDK-58448_1 1\essos-topic-RDK-58448_1"

# Set your Git identity (if not already set)
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# Initialize repository
git init
```

### 3. Create .gitignore File

```bash
# Create .gitignore to exclude build artifacts
cat > .gitignore << 'EOF'
# Build directories
build/
_build/
out/

# Object files
*.o
*.obj
*.elf
*.ko
*.so
*.a
*.lib
*.dll

# Executables
*.exe
*.out
*.app
test_essos_l1
essos_l1_tests

# Test and coverage artifacts
test_results.xml
coverage.info
coverage.cleaned.info
coverage_html/
*.gcno
*.gcda
*.gcov

# Static analysis
static_analysis.log
valgrind_report.xml

# IDE files
.vscode/
.vs/
*.swp
*.swo
*~
.idea/

# OS files
.DS_Store
Thumbs.db
desktop.ini

# Temporary files
*.log
*.tmp
*.bak
EOF
```

### 4. Stage and Commit Files

```bash
# Check status
git status

# Add all files
git add .

# Verify what will be committed
git status

# Create initial commit
git commit -m "Initial commit: Essos L1 Test Suite

- Complete L1 test suite with GoogleTest
- Clean architecture (src/, L1/, mock/)
- CMake and Makefile build systems
- GitHub Actions CI workflow
- Comprehensive documentation
- 100% test pass rate"

# Verify commit
git log --oneline
```

### 5. Push to GitHub

```bash
# Add remote repository (replace with YOUR repository URL)
git remote add origin https://github.com/YOUR_USERNAME/essos-l1-tests.git

# Verify remote
git remote -v

# Rename branch to 'main' (if needed)
git branch -M main

# Push to GitHub
git push -u origin main
```

### 6. Verify GitHub Actions

1. Go to your repository on GitHub
2. Click the **"Actions"** tab
3. You should see:
   - Workflow name: "Essos L1 CI"
   - Status: Running or Completed
4. Click on the workflow run to see details

---

## What GitHub CI Does

### Automatic Triggers

The CI workflow runs automatically when:

✅ **Push to branches**: `main`, `master`, or `develop`  
✅ **Pull requests**: To `main`, `master`, or `develop`  
✅ **Manual trigger**: Via "Actions" tab → "Run workflow"

### CI Workflow Steps

```
┌─────────────────────────────────────┐
│  1. Checkout Code                   │
│     - Downloads your repository     │
└─────────────────────────────────────┘
                ↓
┌─────────────────────────────────────┐
│  2. Install Dependencies            │
│     - build-essential               │
│     - cmake                         │
│     - lcov, genhtml                 │
│     - pkg-config                    │
└─────────────────────────────────────┘
                ↓
┌─────────────────────────────────────┐
│  3. Configure with CMake            │
│     - Creates build directory       │
│     - Enables coverage              │
└─────────────────────────────────────┘
                ↓
┌─────────────────────────────────────┐
│  4. Build Project                   │
│     - Compiles all sources          │
│     - Links test executable         │
└─────────────────────────────────────┘
                ↓
┌─────────────────────────────────────┐
│  5. Run Tests                       │
│     - Executes all L1 tests         │
│     - Generates test_results.xml    │
└─────────────────────────────────────┘
                ↓
┌─────────────────────────────────────┐
│  6. Generate Coverage Report        │
│     - Uses lcov/genhtml             │
│     - Creates HTML report           │
└─────────────────────────────────────┘
                ↓
┌─────────────────────────────────────┐
│  7. Upload Artifacts                │
│     - test-results (XML)            │
│     - coverage-html (HTML)          │
└─────────────────────────────────────┘
```

---

## Viewing Results

### 1. Test Results (XML)

**Location**: Actions → Workflow Run → Artifacts → `test-results`

**Download and View**:
```bash
# Download test-results.zip from GitHub
unzip test-results.zip

# View XML
cat test_results.xml

# Or open in browser/XML viewer
```

**XML Contents**:
- Test suite name
- Number of tests run
- Pass/fail counts
- Execution time per test
- Failure messages (if any)

### 2. Coverage Report (HTML)

**Location**: Actions → Workflow Run → Artifacts → `coverage-html`

**Download and View**:
```bash
# Download coverage-html.zip from GitHub
unzip coverage-html.zip

# Open in browser
# Windows:
start coverage_html/index.html

# Linux/WSL:
xdg-open coverage_html/index.html

# Or manually open coverage_html/index.html in any browser
```

**Coverage Report Shows**:
- Overall coverage percentage
- Coverage by file
- Line-by-line coverage
- Function coverage
- Branch coverage

### 3. Build Logs

**Location**: Actions → Workflow Run → Click on any step

**What to Check**:
- ✅ All dependencies installed
- ✅ CMake configuration successful
- ✅ Build completed without errors
- ✅ All tests passed
- ✅ Coverage generated

---

## Advanced Configuration

### Enable GitHub Pages for Coverage Reports

**1. Generate Coverage Badge**

Add to your `README.md`:
```markdown
![CI Status](https://github.com/YOUR_USERNAME/essos-l1-tests/workflows/Essos%20L1%20CI/badge.svg)
```

**2. Publish Coverage to GitHub Pages**

Modify `.github/workflows/ci.yml`:

```yaml
      - name: Deploy coverage to GitHub Pages
        uses: peaceiris/actions-gh-pages@v3
        if: github.ref == 'refs/heads/main'
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: ./build/coverage_html
```

Then enable GitHub Pages:
- Repository Settings → Pages
- Source: "gh-pages" branch
- Visit: `https://YOUR_USERNAME.github.io/essos-l1-tests/`

### Add Status Checks to Pull Requests

**1. Require Status Checks**

Repository Settings → Branches → Add Rule:
- Branch name pattern: `main`
- ☑ Require status checks to pass before merging
- ☑ Require branches to be up to date before merging
- Select: "Essos L1 CI"

**2. Result**: Pull requests must pass CI before merging

### Run Tests on Multiple Platforms

Modify `.github/workflows/ci.yml`:

```yaml
jobs:
  build-and-test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, ubuntu-20.04, macos-latest]
    steps:
      # ... rest of steps
```

### Add Code Coverage Reporting Service

**Using Codecov**:

```yaml
      - name: Upload coverage to Codecov
        uses: codecov/codecov-action@v3
        with:
          files: ./build/coverage.info
          flags: unittests
          name: codecov-umbrella
```

Then add badge to README:
```markdown
[![codecov](https://codecov.io/gh/YOUR_USERNAME/essos-l1-tests/branch/main/graph/badge.svg)](https://codecov.io/gh/YOUR_USERNAME/essos-l1-tests)
```

---

## Working with GitHub

### Making Changes

```bash
# 1. Make changes to your code
# 2. Check what changed
git status
git diff

# 3. Stage changes
git add src/essos.cpp L1/test_essos_l1.cpp

# 4. Commit
git commit -m "Fix: Corrected display initialization logic

- Fixed null pointer check in EssosContextInit
- Added test case for edge condition
- All tests passing (100%)"

# 5. Push to GitHub
git push
```

### Creating Pull Requests

```bash
# 1. Create new branch
git checkout -b feature/add-new-test

# 2. Make changes and commit
git add L1/test_essos_new_feature.cpp
git commit -m "Add test for new feature XYZ"

# 3. Push branch
git push -u origin feature/add-new-test

# 4. Go to GitHub and create Pull Request
# 5. CI will run automatically on the PR
# 6. Review results before merging
```

### Tagging Releases

```bash
# Create a release tag
git tag -a v1.0.0 -m "Release version 1.0.0

- Complete L1 test suite
- 100% test pass rate
- Full coverage reporting
- GitHub CI integration"

# Push tag to GitHub
git push origin v1.0.0

# GitHub will show it under "Releases"
```

---

## Troubleshooting

### Issue: CI Workflow Not Running

**Check**:
1. Is `.github/workflows/ci.yml` in your repository?
2. Is GitHub Actions enabled? (Settings → Actions → Allow all actions)
3. Did you push to `main`, `master`, or `develop` branch?

**Fix**:
```bash
# Verify workflow file exists
ls -la .github/workflows/

# Check current branch
git branch

# Switch to main branch
git checkout main

# Push again
git push
```

### Issue: Build Fails in CI but Works Locally

**Check**:
1. Are all files committed? (`git status`)
2. Is `.gitignore` excluding necessary files?
3. Are there platform-specific issues?

**Fix**:
```bash
# Check for uncommitted files
git status

# Test locally with same commands as CI
mkdir -p build && cd build
cmake -DENABLE_COVERAGE=ON ..
cmake --build . -- -j$(nproc)
```

### Issue: Tests Fail in CI

**Check**:
1. Do tests pass locally?
2. Are there timing/environment differences?
3. Check CI logs for error messages

**Fix**:
```bash
# Run tests locally first
./build/test_essos_l1 --gtest_output=xml:test_results.xml

# Check for platform-specific issues
# View detailed CI logs on GitHub Actions tab
```

### Issue: Coverage Not Generated

**Check**:
1. Is `lcov` installed in CI? (Check "Install dependencies" step)
2. Was coverage enabled? (`-DENABLE_COVERAGE=ON`)
3. Did tests run before coverage collection?

**Fix**: Coverage should work automatically. Check workflow logs.

### Issue: Cannot Push to GitHub

**Error**: `Permission denied (publickey)`

**Fix**:
```bash
# Use HTTPS instead of SSH
git remote set-url origin https://github.com/YOUR_USERNAME/essos-l1-tests.git

# Or set up SSH key (more secure)
# Follow: https://docs.github.com/en/authentication/connecting-to-github-with-ssh
```

**Error**: `Authentication failed`

**Fix**:
```bash
# Use Personal Access Token instead of password
# Create token at: GitHub → Settings → Developer settings → Personal access tokens
# Use token as password when prompted
```

---

## Best Practices

### ✅ Commit Messages
```bash
# Good
git commit -m "Fix: Resolve memory leak in EssosContextDestroy"

# Better
git commit -m "Fix: Resolve memory leak in EssosContextDestroy

- Added proper cleanup of display resources
- Updated test to verify resource deallocation
- All tests passing"
```

### ✅ Branch Strategy
```
main/master     - Production-ready code
develop         - Integration branch
feature/*       - New features
bugfix/*        - Bug fixes
hotfix/*        - Critical fixes
```

### ✅ Pull Request Workflow
1. Create feature branch
2. Make changes and commit
3. Push and create PR
4. Wait for CI to pass ✅
5. Request code review
6. Merge when approved

### ✅ Regular Testing
```bash
# Before pushing
make clean && make
./test_essos_l1

# Or use helper script
./scripts/run_tests.sh full
```

---

## Summary

### What You Get with GitHub Integration

✅ **Automatic Testing**: Every push runs full test suite  
✅ **Coverage Reports**: Visual coverage reports for every commit  
✅ **Pull Request Checks**: Automated validation before merging  
✅ **Artifact Storage**: Test results and coverage saved  
✅ **Build History**: Track test results over time  
✅ **Collaboration**: Team members can see test status  
✅ **Documentation**: CI badge shows build status  

### Quick Reference Commands

```bash
# Initial setup
git init
git add .
git commit -m "Initial commit"
git remote add origin <your-repo-url>
git push -u origin main

# Daily workflow
git add <changed-files>
git commit -m "Description of changes"
git push

# Create feature branch
git checkout -b feature/new-feature
git push -u origin feature/new-feature

# View CI status
# Go to: https://github.com/YOUR_USERNAME/essos-l1-tests/actions
```

---

## Need Help?

- **GitHub Actions Docs**: https://docs.github.com/en/actions
- **Git Basics**: https://git-scm.com/book/en/v2
- **CI Workflow File**: `.github/workflows/ci.yml` (in your project)
- **Local Testing**: Use `./scripts/run_tests.sh full`

---

**Your GitHub CI is ready to use!** 🚀

Every push will automatically:
1. Build your code
2. Run all tests
3. Generate coverage
4. Upload results as artifacts

Just push your code and let GitHub Actions do the rest!
