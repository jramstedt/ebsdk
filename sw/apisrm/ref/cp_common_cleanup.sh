# Delete the generated files for the platform which was logged in to.

if [ ! -z ${cp_cfgl} ]; then rm ${cp_cfgl}/*; fi
if [ ! -z ${cp_exel} ]; then rm ${cp_exel}/*; fi
if [ ! -z ${cp_incl} ]; then rm ${cp_incl}/*; fi
if [ ! -z ${cp_lisl} ]; then rm ${cp_lisl}/*; fi
if [ ! -z ${cp_objl} ]; then rm ${cp_objl}/*; fi
if [ ! -z ${cp_srcl} ]; then rm ${cp_srcl}/*; fi
