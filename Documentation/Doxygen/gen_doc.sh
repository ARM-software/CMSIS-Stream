#!/usr/bin/env bash
# Generate CMSIS-Stream Doxygen documentation.
#
# Pre-requisites:
# - bash shell
# - doxygen

set -euo pipefail

DIRNAME=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
GENDIR="${DIRNAME}/html"
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
else
  echo "error: doxygen not found in PATH" >&2
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

project_number_escaped=${PROJECT_NUMBER//\\/\\\\}
project_number_escaped=${project_number_escaped//&/\\&}
project_number_escaped=${project_number_escaped//\//\\/}
sed -e "s/{projectNumber}/${project_number_escaped}/g" "${DOXYFILE_IN}" > "${DOXYFILE}"

pushd "${DIRNAME}" >/dev/null

echo "Generating documentation with ${DOXYGEN} ..."
"${DOXYGEN}" "$(basename "${DOXYFILE}")"

mkdir -p "${GENDIR}/search"
cp -f "${DIRNAME}/style_template/search.css" "${GENDIR}/search/"
cp -f "${DIRNAME}/style_template/navtree.js" "${GENDIR}/"
cp -f "${DIRNAME}/style_template/resize.js" "${GENDIR}/"

# Doxygen 1.9.6 can emit literal/escaped <tt> tags for Markdown code spans in
# headings. HTML pages can render those as modern <code>; nav/search scripts
# need plain labels so the tree does not show markup as text.
while IFS= read -r -d '' file; do
  tmp=$(mktemp "${TMPDIR:-/tmp}/cmsis-stream-doxygen.XXXXXX")
  sed -e 's/&lt;tt&gt;/<code>/g' \
      -e 's/&lt;\/tt&gt;/<\/code>/g' \
      -e 's/<tt>/<code>/g' \
      -e 's/<\/tt>/<\/code>/g' \
      "${file}" > "${tmp}"
  cat "${tmp}" > "${file}"
  rm -f "${tmp}"
done < <(grep -IlZ -e '<tt>' -e '</tt>' -e '&lt;tt&gt;' -e '&lt;/tt&gt;' "${GENDIR}"/*.html 2>/dev/null || true)

while IFS= read -r -d '' file; do
  tmp=$(mktemp "${TMPDIR:-/tmp}/cmsis-stream-doxygen.XXXXXX")
  sed -e 's/&lt;tt&gt;//g' \
      -e 's/&lt;\/tt&gt;//g' \
      -e 's/<tt>//g' \
      -e 's/<\/tt>//g' \
      -e 's/&lt;code&gt;//g' \
      -e 's/&lt;\/code&gt;//g' \
      -e 's/<code>//g' \
      -e 's/<\/code>//g' \
      "${file}" > "${tmp}"
  cat "${tmp}" > "${file}"
  rm -f "${tmp}"
done < <(grep -IlZ -e '<tt>' -e '</tt>' -e '&lt;tt&gt;' -e '&lt;/tt&gt;' -e '<code>' -e '</code>' -e '&lt;code&gt;' -e '&lt;/code&gt;' "${GENDIR}"/*.js "${GENDIR}"/search/*.js 2>/dev/null || true)

sed -e "s/{datetime}/${DATETIME}/g" \
    -e "s/{year}/${YEAR}/g" \
    -e "s/{projectName}/CMSIS-Stream/g" \
    -e "s/{projectNumber}/${project_number_escaped}/g" \
    -e "s/{projectNumberFull}/${project_number_escaped}/g" \
    "${DIRNAME}/style_template/footer.js.in" > "${GENDIR}/footer.js"

popd >/dev/null

echo "Documentation generated in ${GENDIR}"
