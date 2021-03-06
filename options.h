/*
 *  OpenVPN-GUI -- A Windows GUI for OpenVPN.
 *
 *  Copyright (C) 2004 Mathias Sundman <mathias@nilings.se>
 *                2010 Heiko Hund <heikoh@users.sf.net>
 *                2016 Selva Nair <selva.nair@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program (see the file COPYING included with this
 *  distribution); if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct connection connection_t;

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <lmcons.h>

#include "manage.h"

#define MAX_NAME (UNLEN + 1)

/*
 * Maximum number of parameters associated with an option,
 * including the option name itself.
 */
#define MAX_PARMS           5   /* May number of parameters per option */
#define MAX_CONFIG_SUBDIRS  50  /* Max number of subdirs to scan for configs */


typedef enum {
    service_noaccess     = -1,
    service_disconnected =  0,
    service_connecting   =  1,
    service_connected    =  2
} service_state_t;

typedef enum {
    config,
    windows,
    manual
} proxy_source_t;

typedef enum {
    http,
    socks
} proxy_t;

/* connection states */
typedef enum {
    disconnected,
    connecting,
    reconnecting,
    connected,
    disconnecting,
    suspending,
    suspended,
    resuming,
    timedout
} conn_state_t;

/* Interactive Service IO parameters */
typedef struct {
    OVERLAPPED o; /* This has to be the first element */
    HANDLE pipe;
    HANDLE hEvent;
    WCHAR readbuf[512];
} service_io_t;

#define FLAG_ALLOW_CHANGE_PASSPHRASE (1<<1)
#define FLAG_SAVE_KEY_PASS  (1<<4)
#define FLAG_SAVE_AUTH_PASS (1<<5)

typedef struct {
    unsigned short major, minor, build, revision;
} version_t;

/* Connections parameters */
struct connection {
    TCHAR config_file[MAX_PATH];    /* Name of the config file */
    TCHAR config_name[MAX_PATH];    /* Name of the connection */
    TCHAR config_dir[MAX_PATH];     /* Path to this configs dir */
    TCHAR log_path[MAX_PATH];       /* Path to Logfile */
    TCHAR ip[16];                   /* Assigned IP address for this connection */
    BOOL auto_connect;              /* AutoConnect at startup id TRUE */
    conn_state_t state;             /* State the connection currently is in */
    int failed_psw_attempts;        /* # of failed attempts entering password(s) */
    time_t connected_since;         /* Time when the connection was established */
    proxy_t proxy_type;             /* Set during querying proxy credentials */

    struct {
        SOCKET sk;
        SOCKADDR_IN skaddr;
        time_t timeout;
        char password[16];
        char *saved_data;
        size_t saved_size;
        mgmt_cmd_t *cmd_queue;
        BOOL connected;             /* True, if management interface has connected */
    } manage;

    HANDLE hProcess;                /* Handle of openvpn process if directly started */
    service_io_t iserv;

    HANDLE exit_event;
    DWORD threadId;
    HWND hwndStatus;
    int flags;
    char *dynamic_cr;              /* Pointer to buffer for dynamic challenge string received */
};

/* All options used within OpenVPN GUI */
typedef struct {
    /* Array of configs to autostart */
    const TCHAR *auto_connect[MAX_CONFIGS];

    /* Connection parameters */
    connection_t conn[MAX_CONFIGS];   /* Connection structure */
    int num_configs;                  /* Number of configs */

    service_state_t service_state;    /* State of the OpenVPN Service */

    /* Proxy Settings */
    proxy_source_t proxy_source;      /* Where to get proxy information from */
    proxy_t proxy_type;               /* The type of proxy to use */
    TCHAR proxy_http_address[100];    /* HTTP Proxy Address */
    TCHAR proxy_http_port[6];         /* HTTP Proxy Port */
    TCHAR proxy_socks_address[100];   /* SOCKS Proxy Address */
    TCHAR proxy_socks_port[6];        /* SOCKS Proxy Address */

    /* HKLM Registry values */
    TCHAR exe_path[MAX_PATH];
    TCHAR global_config_dir[MAX_PATH];
    TCHAR priority_string[64];
    TCHAR ovpn_admin_group[MAX_NAME];
    /* HKCU registry values */
    TCHAR config_dir[MAX_PATH];
    TCHAR ext_string[16];
    TCHAR log_dir[MAX_PATH];
    DWORD log_append;
    TCHAR log_viewer[MAX_PATH];
    TCHAR editor[MAX_PATH];
    DWORD silent_connection;
    DWORD service_only;
    DWORD show_balloon;
    DWORD show_script_window;
    DWORD connectscript_timeout;        /* Connect Script execution timeout (sec) */
    DWORD disconnectscript_timeout;     /* Disconnect Script execution timeout (sec) */
    DWORD preconnectscript_timeout;     /* Preconnect Script execution timeout (sec) */

#ifdef DEBUG
    FILE *debug_fp;
#endif

    HWND hWnd;
    HINSTANCE hInstance;
    BOOL session_locked;
    HANDLE netcmd_semaphore;
    version_t version;
    char ovpn_version[16]; /* OpenVPN version string: 2.3.12, 2.4_alpha2 etc.. */
    unsigned int dpi_scale;
} options_t;

void InitOptions(options_t *);
void ProcessCommandLine(options_t *, TCHAR *);
int CountConnState(conn_state_t);
connection_t* GetConnByManagement(SOCKET);
INT_PTR CALLBACK ScriptSettingsDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ConnectionSettingsDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AdvancedSettingsDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void ExpandOptions(void);
int CompareStringExpanded(const WCHAR *str1, const WCHAR *str2);
#endif
