#
# Copyright (c) 2020 The nanoFramework project contributors
# See LICENSE file in the project root for full license information.
#


# List of the required Sqlite include files.
list(APPEND SQLITE_INCLUDE_DIRS ${PROJECT_BINARY_DIR}/SQLite_Source/src)
list(APPEND SQLITE_INCLUDE_DIRS ${TARGET_BASE_LOCATION})

set(SQLITE_SRCS
    alter.c
    analyze.c
    attach.c
    auth.c
    backup.c
    bitvec.c
    btmutex.c
    btree.c
    build.c
    callback.c
    complete.c
    ctime.c
    date.c
    dbpage.c
    dbstat.c
    delete.c
    expr.c
    fault.c
    fkey.c
    func.c
    global.c
    hash.c
    insert.c
    legacy.c
    loadext.c
    main.c
    malloc.c
    mem0.c
    mem1.c
    mem2.c
    mem3.c
    mem5.c
    memdb.c
    memjournal.c
    mutex.c
    mutex_noop.c
    mutex_unix.c
    mutex_w32.c
    notify.c
    os.c
    os_unix.c
    os_win.c
    pager.c
    pcache.c
    pcache1.c
    pragma.c
    prepare.c
    printf.c
    random.c
    resolve.c
    rowset.c
    select.c
    status.c
    table.c
    tclsqlite.c
    test1.c
    test2.c
    test3.c
    test4.c
    test5.c
    test6.c
    test7.c
    test8.c
    test9.c
    test_async.c
    test_autoext.c
    test_backup.c
    test_bestindex.c
    test_blob.c
    test_btree.c
    test_config.c
    test_delete.c
    test_demovfs.c
    test_devsym.c
    test_fs.c
    test_func.c
    test_hexio.c
    test_init.c
    test_intarray.c
    test_journal.c
    test_loadext.c
    test_malloc.c
    test_md5.c
    test_multiplex.c
    test_mutex.c
    test_onefile.c
    test_osinst.c
    test_pcache.c
    test_quota.c
    test_rtree.c
    test_schema.c
    test_server.c
    test_sqllog.c
    test_superlock.c
    test_syscall.c
    test_tclsh.c
    test_tclvar.c
    test_vdbecov.c
    test_vfs.c
    test_vfstrace.c
    test_windirent.c
    test_window.c
    test_wsd.c
    threads.c
    tokenize.c
    treeview.c
    trigger.c
    update.c
    upsert.c
    utf.c
    util.c
    vacuum.c
    vdbe.c
    vdbeapi.c
    vdbeaux.c
    vdbeblob.c
    vdbemem.c
    vdbesort.c
    vdbetrace.c
    vdbevtab.c
    vtab.c
    wal.c
    walker.c
    where.c
    wherecode.c
    whereexpr.c
    window.c
)

foreach(SRC_FILE ${SQLITE_SRCS})
    set(SQLITE_SRC_FILE SRC_FILE -NOTFOUND)
    find_file(SQLITE_SRC_FILE ${SRC_FILE}
        PATHS 
            ${PROJECT_BINARY_DIR}/SQLite_Source/src

        CMAKE_FIND_ROOT_PATH_BOTH
    )
    # message("${SRC_FILE} >> ${SQLITE_SRC_FILE}") # debug helper
    list(APPEND SQLITE_SOURCES ${SQLITE_SRC_FILE})
endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SQLITE DEFAULT_MSG SQLITE_INCLUDE_DIRS SQLITE_SOURCES)