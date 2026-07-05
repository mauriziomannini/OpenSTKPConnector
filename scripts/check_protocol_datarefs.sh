#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
PROTOCOL_FILE="${ROOT_DIR}/src/protocol.cpp"
DATAREFS_FILE="${ROOT_DIR}/src/datarefs.cpp"
EXPECTED_DUPLICATE="sim/aircraft/parts/acf_gear_deploy"

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "${TMP_DIR}"' EXIT

PROTOCOL_ALL="${TMP_DIR}/protocol-all.txt"
PROTOCOL_UNIQUE="${TMP_DIR}/protocol-unique.txt"
PROTOCOL_DUPLICATES="${TMP_DIR}/protocol-duplicates.txt"
DATAREFS_UNIQUE="${TMP_DIR}/datarefs-unique.txt"
MISSING_RUNTIME="${TMP_DIR}/missing-runtime.txt"
RUNTIME_ONLY="${TMP_DIR}/runtime-only.txt"

sed -nE 's/.*"(sim\/[^"]+)".*/\1/p' "${PROTOCOL_FILE}" > "${PROTOCOL_ALL}"
sort -u "${PROTOCOL_ALL}" > "${PROTOCOL_UNIQUE}"
sort "${PROTOCOL_ALL}" | uniq -d > "${PROTOCOL_DUPLICATES}"

sed -nE 's/.*"(sim\/[^"]+)".*/\1/p' "${DATAREFS_FILE}" | sort -u > "${DATAREFS_UNIQUE}"

comm -23 "${PROTOCOL_UNIQUE}" "${DATAREFS_UNIQUE}" > "${MISSING_RUNTIME}"
comm -13 "${PROTOCOL_UNIQUE}" "${DATAREFS_UNIQUE}" > "${RUNTIME_ONLY}"

if [[ -s "${PROTOCOL_DUPLICATES}" ]]; then
    while IFS= read -r duplicate; do
        if [[ "${duplicate}" != "${EXPECTED_DUPLICATE}" ]]; then
            echo "Error: unexpected duplicate protocol subscription: ${duplicate}"
            exit 1
        fi
    done < "${PROTOCOL_DUPLICATES}"
fi

if [[ -s "${MISSING_RUNTIME}" ]]; then
    echo "Error: protocol advertises DataRefs that are not read at runtime:"
    cat "${MISSING_RUNTIME}"
    exit 1
fi

echo "Protocol/DataRef check passed."
echo "Protocol subscriptions: $(wc -l < "${PROTOCOL_ALL}" | tr -d ' ')"
echo "Unique protocol DataRefs: $(wc -l < "${PROTOCOL_UNIQUE}" | tr -d ' ')"
echo "Runtime DataRefs: $(wc -l < "${DATAREFS_UNIQUE}" | tr -d ' ')"

if [[ -s "${PROTOCOL_DUPLICATES}" ]]; then
    echo "Allowed duplicate subscription: ${EXPECTED_DUPLICATE}"
fi

if [[ -s "${RUNTIME_ONLY}" ]]; then
    echo "Runtime-only DataRefs:"
    cat "${RUNTIME_ONLY}"
fi
