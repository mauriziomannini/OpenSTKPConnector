#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-universal"
BUILD_PLUGIN="${BUILD_DIR}/mac.xpl"
DIST_PLUGIN="${ROOT_DIR}/dist/stkpconnector/mac.xpl"

run_step() {
    echo
    echo "==> $1"
}

require_universal_binary() {
    local path="$1"

    if [[ ! -f "${path}" ]]; then
        echo "Error: expected plugin not found: ${path}"
        exit 1
    fi

    local lipo_output
    lipo_output="$(lipo -info "${path}")"
    echo "${lipo_output}"

    if [[ "${lipo_output}" != *"x86_64"* || "${lipo_output}" != *"arm64"* ]]; then
        echo "Error: ${path} is not a Universal Binary with x86_64 and arm64."
        exit 1
    fi
}

cd "${ROOT_DIR}"

run_step "Checking Bash script syntax"
bash -n scripts/check_protocol_datarefs.sh
bash -n scripts/check_release_ready.sh
bash -n scripts/package_release.sh

run_step "Checking protocol/DataRef consistency"
scripts/check_protocol_datarefs.sh

run_step "Building Universal plugin"
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build "${BUILD_DIR}"

run_step "Checking Universal build output"
require_universal_binary "${BUILD_PLUGIN}"

run_step "Checking dist plugin output"
require_universal_binary "${DIST_PLUGIN}"

run_step "Checking whitespace"
git diff --check

echo
echo "Release readiness check passed."
