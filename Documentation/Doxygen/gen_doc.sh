#!/usr/bin/env bash
# Generate CMSIS-Stream Doxygen documentation.
#
# Pre-requisites:
# - bash shell (for Windows: Git Bash)
# - doxygen

set -euo pipefail

DIRNAME=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
GENDIR="${DIRNAME}/../Doxygen/html"
DOXYFILE_IN="${DIRNAME}/stream.dxy.in"
DOXYFILE="${DIRNAME}/stream.dxy"

usage() {
  echo "Usage: $(basename "$0") [-h]"
  echo " -h,--help               Show usage"
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    -h|--help|help)
      usage
      exit 0
      ;;
    *)
      echo "Invalid command line argument: $1" >&2
      usage
      exit 1
      ;;
  esac
done

if command -v doxygen >/dev/null 2>&1; then
  DOXYGEN=$(command -v doxygen)
elif [[ -x "/c/Program Files/doxygen/doxygen.exe" ]]; then
  DOXYGEN="/c/Program Files/doxygen/doxygen.exe"
else
  echo "error: doxygen not found in PATH or /c/Program Files/doxygen/doxygen.exe" >&2
  exit 1
fi

if command -v git >/dev/null 2>&1; then
  VERSION_FULL=$(git -C "${DIRNAME}/../.." describe --tags --dirty --always 2>/dev/null || true)
fi
VERSION_FULL=${VERSION_FULL:-unknown}
PROJECT_NUMBER="${VERSION_FULL}"
DATETIME=$(date -u +'%a %b %e %Y %H:%M:%S')
YEAR=$(date -u +'%Y')

mkdir -p "${GENDIR}"

sed -e "s/{projectNumber}/${PROJECT_NUMBER}/g" "${DOXYFILE_IN}" > "${DOXYFILE}"

pushd "${DIRNAME}" >/dev/null

echo "Generating documentation with ${DOXYGEN} ..."
"${DOXYGEN}" "$(basename "${DOXYFILE}")"

mkdir -p "${GENDIR}/search"
cp -f "${DIRNAME}/style_template/search.css" "${GENDIR}/search/"
cp -f "${DIRNAME}/style_template/navtree.js" "${GENDIR}/"
cp -f "${DIRNAME}/style_template/resize.js" "${GENDIR}/"

# Doxygen 1.9.6 can emit literal/escaped <tt> tags for Markdown code spans in
# headings. Convert them to modern <code> tags so they are rendered, not shown.
find "${GENDIR}" -type f \( -name '*.html' -o -name '*.js' \) \
  -exec sed -i \
    -e 's/&lt;tt&gt;/<code>/g' \
    -e 's/&lt;\/tt&gt;/<\/code>/g' \
    -e 's/<tt>/<code>/g' \
    -e 's/<\/tt>/<\/code>/g' {} +

sed -e "s/{datetime}/${DATETIME}/g" \
    -e "s/{year}/${YEAR}/g" \
    -e "s/{projectName}/CMSIS-Stream/g" \
    -e "s/{projectNumber}/${PROJECT_NUMBER}/g" \
    -e "s/{projectNumberFull}/${PROJECT_NUMBER}/g" \
    "${DIRNAME}/style_template/footer.js.in" > "${GENDIR}/footer.js"

popd >/dev/null

echo "Documentation generated in ${GENDIR}"
