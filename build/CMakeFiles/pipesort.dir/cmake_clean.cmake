file(REMOVE_RECURSE
  "libpipesort.a"
  "libpipesort.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/pipesort.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
