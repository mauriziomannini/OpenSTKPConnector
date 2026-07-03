#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <version>"
    echo "Example: $0 v0.4.0"
    exit 1
fi

VERSION="$1"
VERSION="${VERSION#v}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-universal"
RELEASE_NAME="OpenSTKPConnector-v${VERSION}-mac-universal"
RELEASE_DIR="${ROOT_DIR}/release/${RELEASE_NAME}"
ZIP_PATH="${ROOT_DIR}/release/${RELEASE_NAME}.zip"
PLUGIN_PATH="${BUILD_DIR}/mac.xpl"

if [[ -z "${XPLANE_SDK:-}" ]]; then
    DEFAULT_XPLANE_SDK="${HOME}/Development/XPSDK"
    if [[ -d "${DEFAULT_XPLANE_SDK}/CHeaders/XPLM" ]]; then
        export XPLANE_SDK="${DEFAULT_XPLANE_SDK}"
        echo "Using default XPLANE_SDK=${XPLANE_SDK}"
    else
        echo "Error: XPLANE_SDK is not set."
        echo "Example: export XPLANE_SDK=\$HOME/Development/XPSDK"
        exit 1
    fi
fi

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"

cmake --build "${BUILD_DIR}"

if [[ ! -f "${PLUGIN_PATH}" ]]; then
    echo "Error: expected plugin not found: ${PLUGIN_PATH}"
    exit 1
fi

LIPO_OUTPUT="$(lipo -info "${PLUGIN_PATH}")"
echo "${LIPO_OUTPUT}"

if [[ "${LIPO_OUTPUT}" != *"x86_64"* || "${LIPO_OUTPUT}" != *"arm64"* ]]; then
    echo "Error: plugin is not a Universal Binary with x86_64 and arm64."
    exit 1
fi

rm -rf "${RELEASE_DIR}" "${ZIP_PATH}"
mkdir -p "${RELEASE_DIR}/stkpconnector"
cp "${PLUGIN_PATH}" "${RELEASE_DIR}/stkpconnector/mac.xpl"

cat > "${RELEASE_DIR}/README.txt" <<EOF
OpenSTKPConnector v${VERSION}
=========================

This archive contains a Universal macOS X-Plane 12 plugin for SimToolkitPro compatibility.

Architectures:
- arm64
- x86_64

Installation
------------

1. Close X-Plane 12.
2. Copy the included "stkpconnector" folder to:

   X-Plane 12/Resources/plugins/

3. The final path must be:

   X-Plane 12/Resources/plugins/stkpconnector/mac.xpl

4. Start X-Plane 12 in native Apple Silicon mode or Intel/Rosetta mode.
5. Start SimToolkitPro.

Known behavior:
- SimToolkitPro may show an "X-Plane Plugin update is required" warning when SimToolkitPro starts. Tracking can still work once X-Plane loads OpenSTKPConnector.
- SimToolkitPro requires the flight to be ended manually from the flight log recorder after X-Plane is closed. This matches the original STKPConnector behavior.
EOF

(
    cd "${ROOT_DIR}/release"
    zip -r "${RELEASE_NAME}.zip" "${RELEASE_NAME}"
)

echo
echo "Release ZIP:"
echo "${ZIP_PATH}"
echo
echo "SHA256:"
shasum -a 256 "${ZIP_PATH}"
