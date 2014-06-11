package com.acer.ccd.cache;

//import com.acer.ccd.debug.L;

import android.net.Uri;

/**
 * Schema for PreviewContent table
 */
public class DBPreviewContent extends DBContent {
//    private static final String TAG = "DBPreviewContent";
    public static final String URI_TAIL = "DBPreviewContent";
    public static final Uri CONTENT_URI = Uri.withAppendedPath(DBProvider.BASE_URI, URI_TAIL);
    public static final String TABLE_NAME = "DBPreviewContent";

    public static final String CREATE_TABLE_SQL =
        "CREATE TABLE "
        + TABLE_NAME
        + CREATE_TABLE_SCHEMA;

    public static final String DELETE_TABLE_SQL =
        "DROP TABLE IF EXISTS "
        + TABLE_NAME;

}
