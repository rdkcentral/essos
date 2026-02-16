# Essos

[![L1 Test Suite](https://github.com/rdkcentral/essos/actions/workflows/L1.yml/badge.svg)](https://github.com/rdkcentral/essos/actions/workflows/L1.yml)
[![Coverity Scan](https://github.com/rdkcentral/essos/actions/workflows/coverity.yml/badge.svg)](https://github.com/rdkcentral/essos/actions/workflows/coverity.yml)

Essos is a library making it simple to create applications that run either as native EGL or Wayland clients. It is a companion to Westeros.

## 🚀 Quick Start

```bash
# One-command setup
./quick-start.sh

# Or manual setup
./scripts/setup-env.sh --install
./scripts/build.sh --coverage
```

## 📊 CI/CD Integration

This repository is integrated with:
- **L1 Test Suite**: Automated testing with coverage reports
- **Coverity Static Analysis**: Code quality and security scanning

**For detailed CI/CD documentation, see [L1_COVERITY_GUIDE.md](L1_COVERITY_GUIDE.md)**

### GitHub Actions Workflows

- **L1 Testing** (`.github/workflows/L1.yml`): Runs on every push/PR
- **Coverity Analysis** (`.github/workflows/coverity.yml`): Weekly + on push to main

### Build Scripts

- `scripts/setup-env.sh` - Environment setup and dependency installation
- `scripts/build.sh` - Main build script with coverage support
- `scripts/coverity-build.sh` - Coverity static analysis build

---
# Copyright and license

If not stated otherwise in this file or this component's Licenses.txt file the
following copyright and licenses apply:

Copyright 2017 RDK Management

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

