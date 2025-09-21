#!/bin/bash

config=${1} ; shift

cpl="cp_common_login.sh"

if [ ! -z ${2} ]; then
  echo "Ignoring: $@ (options are obtained from the build file)."
fi

#
# Find a copy of cp_common_login.sh. Search order is:
#    1. CP_SRC (if defined)
#    2. The current directory.
#    3. The same directory NEWBUILD.COM is running from
#    4. The reference directory.
#
if [ ! -z ${CP_SRC} ]; then
  realcpl=$(realpath "${CP_SRC}/${cpl}")
elif [ -f $(realpath -q "${cpl}") ]; then
  realcpl=$(realpath "${cpl}")
elif [ -f $(realpath -q "${PWD}/${cpl}") ]; then
  realcpl=$(realpath "${PWD}/${cpl}")
elif [ ! -z ${CMSREF} ]; then
  realcpl=$(realpath "${CMSREF}/${cpl}")
else
  echo "Could not find '${cpl}'."
  echo "$0 Aborted."
  exit 1
fi
cpl=${realcpl}

if [ -z ${CP_CFG} ]; then
  echo "Logging in to configuration '${config}'." 1>&2
  source ${cpl} ${config}
else
  mapfile -td'/' <<< "$(realpath "${CP_CFG}")"
  current_config=${MAPFILE[-1]%$'\n'}
  current_platform=${MAPFILE[-3]%$'\n'}

  if [ ! -z ${config} ]; then
    config_name=$(basename "${config}")
    config_name=${config_name%%_*} # Handles newbuild.sh lx164_files.bld 

    if [ "${config_name}" != "${current_config}" ]; then
      echo "CP_CFG is defined as '${CP_CFG}'."
      echo "You asked to build '${config}'."
      echo "To build '${config}' you must first: '${cpl} ${config}'."
      echo "$0 Aborted."
      exit 1
    fi
  fi

  cp_config=${current_config}
  cp_platform=${current_platform}
  echo "Platform = '${cp_platform}', configuration = '${cp_config}'."
fi

if [ -z ${cp_config} ] || [ -z ${cp_platform} ]; then
  echo "Platform and configuration must be nonblank."
  echo "$0 Aborted."
  exit 1
fi

#
# Invoke the build procedure.
#
newbuild_makefile=$(realpath "${CP_SRC}/newbuild.make")

if [ ! -f ${newbuild_makefile} ]; then
  echo "Could not find '${newbuild_makefile}'."
  echo "$0 Aborted."
  exit 1
fi

echo "Executing '${newbuild_makefile}'."

make -f ${newbuild_makefile} PLATFORM=${cp_platform} CONFIG=${cp_config}
exit $?
