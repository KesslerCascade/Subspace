
function(get_all_targets _result _dir)
    get_property(_subdirs DIRECTORY "${_dir}" PROPERTY SUBDIRECTORIES)
    foreach(_subdir IN LISTS _subdirs)
        get_all_targets(${_result} "${_subdir}")
    endforeach()
    get_property(_sub_targets DIRECTORY "${_dir}" PROPERTY BUILDSYSTEM_TARGETS)  #since cmake 3.7
    get_property(_sub_tests   DIRECTORY "${_dir}" PROPERTY TESTS)  #since cmake 3.12
    set(${_result} ${${_result}} ${_sub_targets} PARENT_SCOPE)
    set(${_result}_tests ${${_result}_tests} ${_sub_tests} PARENT_SCOPE)
endfunction()


#organize_targets(...)
#[=[
  organize_targets([TARGETS target1 [target2 ...]]
                   [TGT_REGEX <regex>]
                   [BY_PATH | BY_PARENTPATH | CLEAR_FOLDER]
                   [PREPEND <pre>]
                   [APPEND <post>]
                   [REGEX_REPLACE <regex> <replacement>]

  Todos/Ideas:
                   [TGT_DIR_REGEX <regex>]
                   [BY_TYPE | BY_SHORTTYPE]  # lib / bin                  
                   [JOIN_SPARSE 5]     #integrate folders into parent if less than x targets are present

                   
  Idea for another approach: instead of BY_PARENTPATH, APPEND,... use special format-syntax
                   [FORMAT "%f/%-1p/%t"]  # %f: FOLDER, %p: path, %-1p: parentpath, %t: type of target
                   
                   
Examples:
    organize_targets(BY_PARENTPATH)
    organize_targets(BY_PATH    TARGETS Common )
    organize_targets(TGT_REGEX _Test PREPEND Tests)
    organize_targets(REGEX_REPLACE "^src/" "")
    #organize_targets(TGT_REGEX "^[A-Ga-g]" CLEAR_FOLDER PREPEND "A-G")
    #organize_targets(TGT_REGEX "^[H-Rh-r]" CLEAR_FOLDER PREPEND "H-R")
    #organize_targets(TGT_REGEX "^[S-Zs-z]" CLEAR_FOLDER PREPEND "S-Z")
                   
#]=]
function(organize_targets)
    cmake_parse_arguments(ARG "DEBUG;BY_PARENTPATH;BY_PATH;CLEAR_FOLDER" "TGT_REGEX;PREPEND;APPEND" "TARGETS;REGEX_REPLACE" ${ARGN})
    if(ARG_UNPARSED_ARGUMENTS)
        message(ERROR "organize_targets: Invalid arguments: ${ARG_UNPARSED_ARGUMENTS}")
        return()
    endif()
    if (NOT ARG_TARGETS)
        if(NOT OTGTS_ALLTARGETS)
            get_all_targets(OTGTS_ALLTARGETS ${CMAKE_CURRENT_SOURCE_DIR})
            set(OTGTS_ALLTARGETS ${OTGTS_ALLTARGETS} PARENT_SCOPE)
        endif()
        set(ARG_TARGETS ${OTGTS_ALLTARGETS})
    endif()
    
    if (NOT ARG_TGT_REGEX)
        set(ARG_TGT_REGEX ".*")
    endif()
    
    if (ARG_REGEX_REPLACE)
        list(POP_FRONT ARG_REGEX_REPLACE ARG_REGEX_REPLACE_srch ARG_REGEX_REPLACE_repl)
    endif()       
    
    foreach(target IN LISTS ARG_TARGETS)
        if (target MATCHES "${ARG_TGT_REGEX}")
            get_target_property(type ${target} TYPE)
            if (type STREQUAL "INTERFACE_LIBRARY")  # Cannot set FOLDER for interface_libs
                continue()
            endif()
                
            get_target_property(oldfolder ${target} FOLDER)                
            get_target_property(src ${target} SOURCE_DIR)
            file(RELATIVE_PATH src_rel ${CMAKE_CURRENT_SOURCE_DIR} ${src})
            if(src_rel)
                get_filename_component(src_base ${src_rel} DIRECTORY)
            endif()

            set(fld "${ARG_PREPEND}")
            if(ARG_BY_PARENTPATH)
                list(APPEND fld ${src_base})
            elseif(ARG_BY_PATH)
                list(APPEND fld ${src_rel})
            elseif(NOT ARG_CLEAR_FOLDER)
                list(APPEND fld ${oldfolder})                
            endif()
            list(APPEND fld ${ARG_APPEND})
            string(REPLACE ";" "/" fld "${fld}")
            if (ARG_REGEX_REPLACE_srch)
                string(REGEX REPLACE "${ARG_REGEX_REPLACE_srch}" "${ARG_REGEX_REPLACE_repl}" fld "${fld}")
            endif()           
            set_target_properties(${target} PROPERTIES FOLDER "${fld}")
            if (ARG_DEBUG)
                message(STATUS " ${target}: p: '${src_rel}' f: '${oldfolder}'  ->  '${fld}'")
            endif()
        endif()
    endforeach()
            
endfunction()
