# this hack is required to make the search for Git path to work in Windows platforms
# it can safelly be removed after:
# https://gitlab.kitware.com/cmake/cmake/merge_requests/332 is MERGED
# and makes it to a release version of CMake, then we need to have that version as minimum required
# 

if(CMAKE_HOST_WIN32)
  if(NOT CMAKE_GENERATOR MATCHES "MSYS")
    set(git_names git.cmd git eg.cmd eg)
    # GitHub search path for Windows
    file(GLOB github_path
      "$ENV{LOCALAPPDATA}/Github/PortableGit*/cmd"
      "$ENV{LOCALAPPDATA}/Github/PortableGit*/bin"
      )
    # SourceTree search path for Windows
    set(_git_sourcetree_path "$ENV{LOCALAPPDATA}/Atlassian/SourceTree/git_local/bin" CACHE STRING "caching Git source tree path for latter use in FindGit" FORCE)
  endif()
endif()
