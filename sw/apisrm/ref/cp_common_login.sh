#!/bin/bash

# [ ${cp$v_cpl} == 1 ] && set -x

ask_mkdir()
{ 
  if [ -d $1 ]; then return; fi
  if [ -z $2 ] || [ $2 == false ] ; then mkdir $1; return; fi

  read -p "Create directory '$1'? (y/n) " yn
  case $yn in
    [Yy]* ) mkdir $1;;
    [Nn]* ) exit;;
    * ) echo "Please answer yes or no.";;
  esac
}

#         1       2            3         4                    5
# defsrch CP_SRC "${root}/src" "CP\$REF" "./src ./common/src" "."
# defsrch CP_OBJ "${root}/obj" "CP\$REF" "./obj"
defsrch()
{
  local env=${1}  # Environment variable to export
  local head=${2} # Head of search list
  local back=(${3}) # Backing list to expand
  local nref=(${4}) # List of non ref relative paths
  local ref=(${5})  # Alternate list for CP$REF relative paths

  exp=""

  for b in ${back[@]}; do
    if [ $b = "CP\$REF" ]; then
      ctl=${ref}
      b=${CP_REF}
    else
      ctl=${nref}
    fi

    for c in ${ctl}; do
      if [ -z ${c} ]; then continue; fi

      c=$(realpath -s "${b%%/}/${c}")

      if [ -z ${exp} ]; then
        exp=${c}
      else
        exp="${exp}:${c}"
      fi
    done
  done

  if [ -z ${head} ]; then
    export ${env}="${exp}"
  else
    if [ -z ${exp} ]; then
      export ${env}="${head}"
    else
      export ${env}="${head}:${exp}"
    fi
  fi
}

configfile=$1
default=$(realpath ${2:-${PWD}})
back=$3

#
# If the default directory wasn't specified, make some educated guesses as
# to where we should be.
#
if [ ! -d ${default} ]; then
  echo "Could not find working directory '${default}'."
  exit
fi

#
# If the default or requested default is named "src", go up one level.
#
if [ ${default##*/} == "src" ]; then
  default=$(dirname ${default})
fi

#
# Check baseline source reference.
#
if [ -z ${CP_REF} ]; then
  echo "No CP_REF environment set."
  echo "$0 Aborted."
  exit 1
else
  echo "Baseline source reference is in '${CP_REF}'."
fi

#
# Set up root directory name.
#
root=${default}

if [ -z ${configfile} ]; then
  unset cp_arch
  unset cp_platform
  unset cp_config
else
  if [ -z ${configfile##*/*} ]; then
    configfile=$(realpath ${configfile})
  else
    name=${configfile%%.*}
    ext=${configfile#*.}

    if [ ! -z ${name##*_*} ]; then
      name="${name}_files"
    fi

    if [ ! -z ${configfile##*.*} ]; then
      ext="bld"
    fi

    filename="${name}.${ext}"

    if [ ! -z ${CP_SRC} ]; then
      configfile=$(find ${CP_SRC//:/\/ } -name ${filename} | head -1)
      #configfile=$(realpath "${CP_SRC}/${filename}")
    elif [ -f $(realpath -q "${root}/src/${filename}") ]; then
      configfile=$(realpath "${root}/src/${filename}")
    elif [ ! -z ${CP_REF} ]; then
      configfile=$(realpath "${CP_REF}/${filename}")
    fi
  fi

  if [ ! -f ${configfile} ]; then
    echo "Could not find configuration file '${configfile}'."
    configfile="${configfile}.bld"
    echo "Using '${configfile}' as platform name."
    export cp_arch="ALPHA"
    export cp_platform=${configfile,,}
    export cp_config=${configfile,,}
  fi


  if [ -z ${cp_platform} ] || [ -z ${cp_config} ] || [ -z ${cp_arch} ]; then
    #
    # Look for bldcpl.awk
    #
    bldcpl="bldcpl.awk"
    if [ ! -z ${CP_SRC} ]; then
      bldcpl=$(find ${CP_SRC//:/\/ } -name ${bldcpl} | head -1)
      #bldcpl=$(realpath "${CP_SRC}/${bldcpl}")
    elif [ -f $(realpath -q "${root}/src/${bldcpl}") ]; then
      bldcpl=$(realpath "${root}/src/${bldcpl}")
    elif [ ! -z ${CP_REF} ]; then
      bldcpl=$(realpath "${CP_REF}/${bldcpl}")
    else
      echo "Could not find file '${bldcpl}'."
      exit 1
    fi

    scratch=$(mktemp -t 'cpl_XXXXXXXXXX')

    #
    # If CP_SRC is undefined, define it to be something.
    # This is needed for processing of include directives in the .bld file.
    #
    if [ -z ${CP_SRC} ]; then
      export CP_SRC="${root}/src:${CP_REF}" 
    fi

    #
    # Parse the configuration file, and use a awk output script to generate
    # a shell stub which exports environment variables for the architecture, platform,
    # and configuration.
    #
    echo "Reading configuration file '${configfile}'."
    awk -f "${bldcpl}" < "${configfile}" > "${scratch}"
    source "${scratch}"
    rm "${scratch}"

    if [ -z ${cp_platform} ]; then export cp_platform=${name%%_*}; fi
    if [ -z ${cp_config} ]; then export cp_config=${name%%_*}; fi
    if [ -z ${cp_arch} ]; then export cp_arch="ALPHA"; fi
  fi

  #
  # Announce the platform.
  #
  if [ ${cp_platform} = ${cp_config} ]; then
    echo "Platform is '${cp_platform}' (base configuration), architecture is '${cp_arch}'."
  else
    echo "Platform is '${cp_platform}', configuration is '${cp_config}', architecture is '${cp_arch}'."
  fi

  #
  # If the current directory has the platform name, go up one level.
  #
  if [ ${default##*/} == ${cp_platform} ]; then
    default=$(dirname ${default})
    root=${default}
  fi

  rootp="${root}/${cp_platform}"

  #
  # If the platform work directory does not already exist, then ask if it
  # should be created.  If the answer is "No", then the user specified the
  # wrong work directory (or set default to the wrong place), so just abort.
  #
  if [ ! -d ${rootp} ]; then ask_mkdir "${rootp}" true; fi
  if [ ! -d ${rootp} ]; then
    echo "$0 Aborted."
    exit 1
  fi
fi

if [ ! -z ${back} ]; then
  echo "Backing not supported."
  exit
fi

#
# Create the platform subdirectories.
#
if [ ! -z ${cp_platform} ]; then
  ask_mkdir "${rootp}/src"
  ask_mkdir "${rootp}/obj"
  ask_mkdir "${rootp}/lis"
  ask_mkdir "${rootp}/exe"
  ask_mkdir "${rootp}/log"
  ask_mkdir "${rootp}/cfg"
  ask_mkdir "${rootp}/cfg/${cp_config}"
  ask_mkdir "${rootp}/cfg/${cp_config}/inc"
  ask_mkdir "${rootp}/cfg/${cp_config}/log"

  #
  # The base configuration drops its files in the old place, for
  # compatability with the old scheme of things.  Don't create config
  # directories which won't be used.
  #
  if [ ${cp_platform} != ${cp_config} ]; then
    ask_mkdir "${rootp}/cfg/${cp_config}/exe"
    ask_mkdir "${rootp}/cfg/${cp_config}/lis"
    ask_mkdir "${rootp}/cfg/${cp_config}/obj"
  fi
fi

#
# Define build environment.
#

#
# Different depending on whether a platform was specified.
#
if [ -z ${cp_platform} ]; then
  # TODO backlist
  defsrch CP_SRC "${root}/src" "CP\$REF" "./src ./common/src" "."
else
  defsrch CP_CFG "${rootp}/cfg/${cp_config}" "CP\$REF" "${cp_platform}/cfg/${cp_config}"
  defsrch CP_SRC "${rootp}/src:${root}/src:${CP_CFG}" "CP\$REF" "./${cp_platform}/src ./src ./common/src" "."
  defsrch CP_LOG "${rootp}/log" "CP\$REF" "./${cp_platform}/log"

  if [ ${cp_config} = ${cp_platform} ]; then
    # This is the base configuration
    export cp_cfgl="${rootp}/cfg/${cp_config}"
    export cp_exel="${rootp}/exe"
    export cp_incl="${rootp}/cfg/${cp_config}/inc"
    export cp_lisl="${rootp}/lis"
    export cp_objl="${rootp}/obj"
    export cp_srcl="${rootp}/src"

    defsrch CP_INC "${rootp}/cfg/${cp_config}/inc" "CP\$REF" "./${cp_platform}/cfg/${cp_config}/inc"
    defsrch CP_EXE "${rootp}/exe" "CP\$REF" "./${cp_platform}/exe"
    defsrch CP_LIS "${rootp}/lis" "CP\$REF" "./${cp_platform}/lis"
    defsrch CP_OBJ "${rootp}/obj" "CP\$REF" "./${cp_platform}/obj"
  else
    # This is not the base configuration
    rc="${rootp}/cfg/${cp_config}"
    rp="${rootp}/cfg/${cp_platform}"
    export cp_cfgl="${rc}"
    export cp_exel="${rc}/exe"
    export cp_incl="${rc}/inc"
    export cp_lisl="${rc}/lis"
    export cp_objl="${rc}/obj"
    export cp_srcl="${rc}/inc"

    defsrch CP_INC "${rc}/inc:${rp}/inc" "CP\$REF" "./${cp_platform}/cfg/${cp_config}/inc ./${cp_platform}/cfg/${cp_platform}/inc"
    defsrch CP_EXE "${rc}/exe:${rootp}/exe" "CP\$REF" "./${cp_platform}/cfg/${cp_config}/exe ./${cp_platform}/exe"
    defsrch CP_LIS "${rc}/lis:${rootp}/lis" "CP\$REF" "./${cp_platform}/cfg/${cp_config}/lis ./${cp_platform}/lis"
    defsrch CP_OBJ "${rc}/obj:${rootp}/obj" "CP\$REF" "./${cp_platform}/cfg/${cp_config}/obj ./${cp_platform}/obj"
  fi
fi

#
# Export environment variable cplogin to invoke this procedure.
#
alias cplogin=$(find ${CP_SRC//:/\/ } -name $(basename "$BASH_SOURCE") | head -1)
#
# Export environment variable cpsetup to invoke a procedure to set up the cross-
# compilation tools.
#
alias cpsetup=$(find ${CP_SRC//:/\/ } -name console_tools.sh | head -1)
#
# #environment variable to clean up generated files from the current build tree.
#
alias cpcleanup=$(find ${CP_SRC//:/\/ } -name cp_common_cleanup.sh | head -1)

#
# Go to the user source directory, if it exists.  Otherwise, go to the
# platform source directory.
#
ask_mkdir "${root}/src" true
if [ -d "${root}/src" ]; then
  cd "${root}/src"
elif [ -d "${rootp}/src" ]; then
  cd "${rootp}/src"
else
  echo "Directory '${root}/src' or '${rootp}/src' does not exist -- current working directory unchanged"
fi

# Normal exit
pwd
return 0
