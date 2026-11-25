var ipmonitor_last = [<% get_static_client(); %>];
var ipv6_clients_last = <% get_ipv6_clients(); %>;
var nmap_fullscan_last = '<% nvram_get_x("", "networkmap_fullscan"); %>';