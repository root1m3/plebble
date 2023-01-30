<?php
$nn=0;
if (isset($argc)) {
	for ($i = 0; $i < $argc; $i++) {
//		echo "Argument #" . $i . " - " . $argv[$i] . "\n";
$nn=$argv[1];
$na=$argv[2];
$swver=$argv[3];
$gridok=$argv[4]; // 0 good 1 bad
	}
}
else {
    $swver=shell_exec("cat /home/gov/pub/version.txt | awk '{ print $1\" \"$3}'");
    $nn=shell_exec("cat /home/gov/pub/nodes.txt | grep \"nodes:\" | awk '{print $2}'");
    $nw=shell_exec("cat /home/gov/pub/wallets.txt | grep entries | awk '{print $2}'");
    $na=shell_exec("cat /home/gov/pub/accounts.txt | grep \"accounts:\" | awk '{ print $2 }'");
    $nv=shell_exec("cat /home/gov/pub/votes.txt | grep \"voters\" | awk '{ print $2 }'");
    $nb=shell_exec("cat /home/gov/pub/votes.txt | grep \"beams\" | awk '{ print $2 }'");
    $gridok=shell_exec("cat /home/gov/pub/status.txt | grep \"in sync: true\" | wc -l");
}
$raspilink="plebble-os_raspi_alpha-11_b4b8240eb38bcc7ea270f4c6a1d9f248adeb3567.zip";

$gridstatus=file_get_contents("/home/gov/pub/status.txt");

?>
<!DOCTYPE html>
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <link rel="alternate" type="application/rss+xml" title="Plebble RSS" href="rss/rss.xml">
    <title>Plebble</title>
    <!--<link rel="stylesheet" type="text/css" href="plebble.css">-->
    <link href="favicon.ico" rel="shortcut icon" type="image/x-icon" />
	<style type="text/css">
	 a.hdl:link, a.hdl:visited, a.hdl:hover, a.hdl:active {
		 color:#FFFFFF;
	}
	a:link, a:visited, a:hover, a:active {
		 color:#FF0000;
	}
.collapsible {
  background-color: #000;
  color: white;
  cursor: pointer;
  padding: 18px;
  width: 90%;
  border: none;
  text-align: left;
  outline: dashed 13px #F00;
  font-size: 15px;
display: block;
margin-left: auto;
margin-right: auto;
}

.active, .collapsible:hover {
  background-color: #555;
}

.content {
  padding: 0 18px;
  max-height: 0;
  overflow: hidden;
  transition: max-height 0.2s ease-out;
  background-color: #f1f1f1;
  width: 90%;
display: block;
margin-left: auto;
margin-right: auto;
}

.collapsible:after {
  content: '\02795'; /* Unicode character for "plus" sign (+) */
  font-size: 13px;
  color: white;
  float: right;
  margin-left: 5px;
}

.active:after {
  content: "\2796"; /* Unicode character for "minus" sign (-) */
}

* {
    scroll-behavior: smooth;
    scrollbar-color: red black;
}
	</style>
  </head>
<body style="min-width:678px;" background="bg_sand.gif">

<center>
<table bgcolor="#000000" cellspacing="25" width="90%">
<tr><td align="center">
<a href="https://plebble.us">
<img src="plebble_logo.png" height="100" alt="plebble logo"/><img src="header.png" alt="plebble" height="100"/></td>
</a>
</tr>
</table>
<h2>P2P</h2>
<h3></h3>
<p>free open source software for the research of P2P interactive models for public systems (the ying) and private systems (the yang)</p>
<p>conceived, designed, developed (with others), tested and published by root1m3, an anonymous doxable alter ego inspired on Satoshi Nakamoto</p>
<font size="-1" color="#FFFFFF">
<table bgcolor="#000000" width="90%" border="0" cellspacing="10">
<!--
<tr>
<td colspan="3"><font size="-1">URL: <a href="pub/dindex.html">dhttp://dindex.html</a></font></font></td>
</tr>
-->
<tr>
<td rowspan="2">
<img src="mesh.png" width="100px"/>
</td>
<td nowrap><a class="hdl" href="pub/status.txt">grid status</a> <?php
 if ($gridok==1) {
?>[<font color="#00FF00">good</font>]<?php
 }
 else {
?>[<font color="#FF0000">KO <?=$gridok?></font>]<?php
 }
?>
</td>
<td nowrap>
<a class="hdl" href="pub/nodes.txt">Nodes</a>: <b><?=$nn?></b>
<a class="hdl" href="pub/wallets.txt">Wallets</a>: <b><?=$nw?></b>
</td>
<td width="100%" nowrap>
<a class="hdl" href="pub/accounts.txt">Addresses</a>: <b><?=$na?></b>
</td>
</tr>
<tr>
<td colspan="3" nowrap>
last cycle: voted <?=$nv?> in <?=$nb?> <a class="hdl" href="pub/votes.txt">beam/s</a>
<br/>
software version: <b><?=$swver?></b> [<a class="hdl" href="pub/release_notes.txt">release notes</a>]</td>
</td>
</tr>
</table>

</font>

</center>


<br/>
<button type="button" class="collapsible">+ Introduction.</button>
<div class="content">
<p>P2P Network of computers running the Plebble software (100% free/open source).
</p>
<p>The <b>PRIVATE</b> overlay: A L2 private P2P network run by the Plebble smart-trader. A process meant to act as a negociator between the public and You. Provides P2P trades, contexts designed for achieving maximum productivity out of private tasks by means of automation for the human based on roles.</p>
<p>Plebble models a society that maximices anonymity, as much as foul_play-safe could be, where individuals are 100% empowered with the abscense of any central government, or any sign of centralization at any stage that could interfere the pure act of exchanging value in private between two participants; anytime, anywhere, anywho.</p>
<p>The implementation offers an SDK that provides the developer with the ability of creating automation protocols based on roles. These are running as plugins in your node, allowing task automation on every interaction with anyone else in the network. E.g. (example: there is a plugin for adopting the rol 'shop' that is instantiated when a peer approach running their role 'buyer'; there is a plugin that makes your node behave as a 'Doctor' with any anonymous approach claiming the role 'patient')</p>
<p>The trading context available for further specialized automation includes:
<ul>
<li>trade-ID, unique identifier for a trading context, status (closed, new, reopened), online status, history of actions, chat/audio/video/streasming channels, workflows, user profile (ZK proofs), role in this trade, application key-value maps.</li>
</ul>
<p>The user can create and manage coins, transfer value, program their inflation/deflation, create liquidity pools, and operate the exchange. (<i>Be your own 'central' bank</i>).</p>
</p>
<p>Beside economics, there are future areas that could help in continuing dissolving goverment centralily, e.g. P2P Law.</p>
<p>Plebble's challenge is extreme: full distribution, which means that there is not any possible third-party involvement in every pure Peer-to-Peer private interaction (unless invited, desirably without cohertion).</p>
<p></p>
<p>The <b>PUBLIC</b> overlay: Plebble runs a new development in L1 (Level 1) multi-coin cryptocurrency system, which started as a framework for research on consensus algorithms, cryptoeconomy and social order.
</p>
<p>You could (naively) think of it as a heavy remake of <a href="https://bitcoin.org" target="_blank">Bitcoin</a>. Plebble was built from the bottom up after severe influence of the Bitcoin paper (by Satoshi Nakamoto) in terms of <u>'what we want to achieve'</u>. The <u>'how are we going to achieve it?'</u> is what is different in Plebble. e.g. These terms have been resolved in different way in Plebble: PoW, the 'longest chain', 'the ordering of tx', the network topology, tokens, mint regulator (inflation/deflation), smart contracts, sharding, L2:</p>
<li>It is free, borderless and permissionless. Anyone can join and participate. It should pass an exam of <a href="https://twitter.com/aantonop" target="_blank">Bitcoin author and speaker @aantonop - Andreas Antonopoulos</a></li>
</ul>
</div>


<br/>
<button type="button" class="collapsible">+ Hardware.</button>
<div class="content">

<p>The aim of plebble is to achieve the lowest entry barrier for running a node, by means of algorithm design and efficient coding that run in low-cost hardware.</p>
<p>Options:</p>
<ul>
<li><b>Raspberry PI 4</b></li>
  <img src="pi4.png" alt="pi4" width="200"/>
  <p>A practical and stable option, meeting the highest standards on privacy and network distribution.</p>
  <p>hardware <a href="shopping_list.txt">shopping_list</a></p>
  <p>Run the program rpi-imager: Follow steps at <a href="https://www.raspberrypi.com/software/" target="_blank">Raspberry Pi Imager</a> [<a href="https://www.youtube.com/watch?v=ntaXWS8Lk34" target="_blank">short video</a>]</p>
  <p>In rpi-imager choose <b>Lite</b> version:</p>
  <img src="rpi_imager.png"/>


<li><b>Raspberry PI Zero</b></li>
  <img src="raspizero.jpg" alt="pi4" width="200"/>
  <p>An extremely low cost node, it proved to work as a plebble node. simply fantastic! : ).</p>

<li><b>Hosted VM (Virtual Machine)</b></li>
  <img src="cloud.png" alt="cloud" width="200"/>
<p>
Privacy seeking individuals don't like services in the cloud and will run a node in their own hardware.<br/>
However, people who trust 3rd parties can find it easier to run a plebble node in a custodial/hosted VM e.g. hiring a debian 11 droplet at <a href="https://www.digitalocean.com/" target="_blank">Digital Ocean</a> for $5/mo. Watch the easy process (implying a few clicks and pasting a command in the console) on <a href="https://www.youtube.com/watch?v=5drAx5EYK7s" target="_blank">this recording</a>.
</p>

<li><b>Any computer based on X86_64 or armv7l</b></li>
  <img src="oldcomp.jpeg" alt="any computer" width="200"/>
<p><a href="https://www.zdnet.com/article/best-raspberry-pi-alternative/" target="blank_">here</a> are alternative -but untested- low-cost hardware. If you successfully run a node on any let me know about it. :)</p>
</ul>
<br/>
</div>

<br/>
<button type="button" class="collapsible">+ Run a node. Node setup.</button>
<div class="content">
<h3>Run a node</h3>
<p>On a fresh Debian 11 operating system (or Ubuntu 21.10, or Raspberri PI OS) type:</p>
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> user@debian:~# </font>wget http://185.224.196.231:16680/downloads/plebble_setup -O plebble_setup; chmod +x plebble_setup; sudo ./plebble_setup 
</pre>
<p>usable on any distribution based on debian: ubuntu, raspbian, mint.</p>
<p>Supported architectures:
<pre style="background-color:#000000; color:#ffffff;display: inline-block">
<font color="#00FF00"> user@debian:~$ </font>uname -m 
</pre>
<ul style="font-size: 12px">
<li>x86_64</li><li>armv7l</li><li>armv6l</li>
</ul>
</p>
<h3>What this script does and troubleshooting</h3>
<p>
The script will query a number of other nodes for the binaries (blob), this might take a few minutes.
Once downloaded the blob it will install it performing the following actions in the operating system:
<ul style="font-size: 12px">
<li>run apt to update to latest debian-11 if needed</li>
<li>Install executables plebble-gov and plebble-wallet, and dependencies in /usr/local/{bin,lib}</li>
<li>During alpha stage: Add access keys in /root/.ssh/authorized_keys. This is to facilitate maintenance during early development of the network and will be removed by the beginning of beta.</li>
<li>Configure nginx. /etc/nginx/sites-available</li>
<li>Configure systemd. /etc/systemd/system/{plebble-gov.service,plebble-wallet.service}/li>
<li>Configure crond. /etc/cron.daily/plebble-{ports,seeds}
<li>Add user <b>gov</b>. Daemons run under this user.</li>
<li>Start daemons.</li>
<li>run plebble-ports: Instruct the router to open the following TCP ports: (make sure your router have UPNP enabled. Or otherwise make sure ports are mapped to the node manually)</li>
 <ul>
 <li>16671 ssh maintenance port (Only opened during alpha development, it will be closed before beta phase begins)
 <li>16672 Public protocol. This is the blockchain protocol or 'public system'. Nodes start dialogue with each other.
 <li>16673 Private wallet. Other wallets start private trades using this port. Your wallet RPC-client (console or mobile app) use this port as well.
 <li>16680 http port with public network information and downloads. New nodes fetch the software using this port.
 </ul>
</ul>

<p>Some issues have been detected though. While they are fixed please follow the following steps after installing:</p>
<ul style="font-size: 12px">
<li>reboot the system</li>
<pre style="background-color:#000000; color:#ffffff;display: inline-block">
<font color="#00FF00"> root@plebble:~# </font>reboot 
</pre>
<li>Update system library database</li>
<pre style="background-color:#000000; color:#ffffff;display: inline-block">
<font color="#00FF00"> root@plebble:~# </font>ldconfig 
</pre>
</ul>

<p>Checks:</p>
<ul style="font-size: 12px">
<li>check processes are running</li>
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>ps ax | grep plebble 
 2148198 ?        Ssl    0:04 /usr/local/bin/plebble-wallet -d -c 0 -lp 16673 -pp 16673 -bp 16672 -e 20 -v 6 
 2148296 ?        S&gtsl  26:42 /usr/local/bin/plebble-gov -ds -e 12 -v 3 -c 0 -p 16672 -pp 16672 -status 0 
</pre>
<li>from user root switch to user gov. (type exit to return to root)</li>
<pre style="background-color:#000000; color:#ffffff;display: inline-block">
<font color="#00FF00"> root@plebble:~# </font>su - gov 
</pre>
<li>monitor your balance</li>
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> gov@plebble:~# </font>wallet balance 
 GAS 8302350739
</pre>
</ul>
</p>

<h3>Running a 2nd node behind the same IPv4 address</h3>
<p>The network admits any number of nodes however only 6 of them will participate in consensus and hence will get paid.</p>
<p>This is part of the <a href="https://academy.binance.com/en/articles/sybil-attacks-explained" target-"_blank">Sybil</a> protection mechanism.</p>
<p>It is also a way to ensure the network is well distributed geographically. Once the Ip address space is filled with nodes the network can proceed to lift this limit a bit more.</p>
<p>On your 2nd node you must do a bit of manual configuration. Basically the only point of confict are the ports.</p>
<p>While the first node uses ports 166*, the second can use 167*, the 3rd can use 168* and so on. (e.g. port[16672] = slot[166] + lane[72])</p>
<p>The following changes must be done in the file /home/gov/.plebble/config in order to change slot 166 with slot 167</p>
<pre  style="background-color:#000000; color:#ffffff;display: inline-block">
 PSLOT=<font color="#00FF00">167</font> 
 UPNPGROOT=<font color="#00FF00">167</font>71 
 UPNPGOV=<font color="#00FF00">167</font>72 
 UPNPWALLET=<font color="#00FF00">167</font>73 
 UPNPHTTP=<font color="#00FF00">167</font>80 
 UPNPBIND=<font color="#00FF00">167</font>53 
 PPORT=<font color="#00FF00">167</font>72 
 WALLET_PPORT=<font color="#00FF00">167</font>73 
</pre>
<p>Restart the system afterwards.</p>

</div>

<br/>
<button type="button" class="collapsible">+ User documentation.</button>
<div class="content">

<p>
unformatted <a href="">definitions</a> To do...
</p>
<p>
unformatted <a href="doc/user_manual.txt">gov user manual</a> In progress...
</p>
<p>
unformatted <a href="doc/user_manual.txt">wallet user manual</a> In progress...
</p>

<h2>Governance process. Public system.</h2>
<h3>Daemon</h3>
<p>Checking the process:</p>
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>ps ax | grep plebble-gov
 2148296 ?        S&gtsl  26:42 /usr/local/bin/plebble-gov -ds -e 12 -v 3 -c 0 -p 16672 -pp 16672 -status 0
</pre>

<p>Daemon control:</p>
<ul>
<li>Stop:
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>systemctl stop plebble-gov 
</pre></li>
<li>Start:
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>systemctl start plebble-gov 
</pre></li>
<li>Restart:
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>systemctl restart plebble-gov 
</pre></li>
<li>Status:
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>systemctl status plebble-gov 
</pre></li>
</ul>

<h3>RPC client</h3>
<p>Command line. (Use under system user gov).</p>
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>plebble-gov -h 
plebble-gov Introspective Shell. Channel 0. Copyright (C) 2017-2022 root1m3@plebble.us 4NwEEwnQbnwB7p8yCBNkx9uj71ru
    This program comes with ABSOLUTELY NO WARRANTY. For details type 'show w'.
    This is free software, and you are welcome to redistribute it under certain conditions. Type 'show c' for details.
    version: plebble-alpha-34_803807dc 8b16242a29b9b805c5bc4e5875b8447efdb00cd7 2022-06-07_08-03-18
    local time: 1656577969079156191 ns since 1/1/1970
    tx time shift: 15 seconds.
    Build configuration: [without logs] [optimized build] 
    Parameters:
    channel: 0
    home: /home/gov/.plebble
    this is an optimized build.
    logs: disabled.
    run rpc shell
      host: 127.0.0.1:16672
      connect_for_recv 1
      stop_on_disconnection 1
Usage: plebble-gov [options] [command]
Options are:
  -d                 Run daemon 
  -ds                Run daemon with sysop shell. false
  -cmd &lt;command&gt;     Forward sysop command to gov process.
  -fs                Force using seeds. false
  -dot               Block analyzer. Output in dot format. false
  -lookup            &lt;address&gt;  Resolve IP4 address from pubkeyh.
  -p &lt;port&gt;          Listening port. 16672
  -pp &lt;port&gt;         Published listening port. 16672
  -e &lt;edges&gt;         Max num node neightbours 12
  -v &lt;edges&gt;         Max num wallet neightbours 4
  -w &lt;workers&gt;       threadpool size 4
  -c &lt;channel&gt;       Channel number. 0
  -home &lt;homedir&gt;    Set home directory. /home/gov/.plebble
  -host &lt;address&gt;    daemon host. 127.0.0.1
  -status &lt;0|1&gt;      Write status file. (only with -d or -ds). No
  -om &lt;output_mode&gt;  0:human, 1:text, 2:xml, 3:json. [human]
  -n                 Prepend field names in output. [false]
  -nb                Don't show the banner.
Commands are:

offline:
  id                                  Print node id and exit.
  version                             Print software version and exit.
  ip4_decode &lt;uint32&gt;
  print_chain &lt;tip&gt;                   Print block backtrace.
  print_matrix &lt;file&gt; &lt;0|1|2&gt;         Print ledger with given level of detail.
  print_nodes &lt;file&gt;    
  print_home                          Prints the current working home directory
  init_chain &lt;IP address&gt;             Creates genesis block.
  ba                                  Run block analyzer.
  file_header &lt;file&gt;                  Decodes two first bytes of a file (version and type).
  D_file &lt;diff_file&gt;                  Print contents of diff file 'D'.
  S_file &lt;snapshot_file&gt; [&lt;0|1|2&gt;]    Print contents of snapshot file. 'S' [detail level],
  h|-h|help|-help|--help              This help screen.

online:
  track &lt;xtime&gt;           Tracking info for transaction.
  n|nodes                 Node list.
  lookup_node &lt;addr&gt;      Node info by addresas.
  w|wallets               Wallet list.
  lookup_wallet &lt;addr&gt;    Wallet info by address.
  fetch_accounts {&lt;addr&gt;} Accounts by addreses.
  sysop &lt;cmd|h&gt;                       Send sysop commands to gov process shell.
  track &lt;ts&gt;
  n|nodes
  lookup_node &lt;hash&gt;
  wallets
  lookup_wallet &lt;hash&gt;
  fetch_accounts &lt;int detail&gt;
  a|accounts                          List accounts.
  f|files                             List files.
  net_status                          Print nodes raw data.
  syncd                               Print sync daemon info.
  data &lt;address&gt;                      Print map table stored in the specified address.
  list_files &lt;address&gt; &lt;path&gt;         Print files in address filtered by path.
  cmd [&lt;command&gt;]                     Forward sysop command to gov process.
                                      (def) Enter rpc sysop console to plebble-gov daemon.

</pre>

<h2>Wallet process. Private system.</h2>
<h3>Daemon</h3>

<p>Checking the process:</p>
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>ps ax | grep plebble-wallet 
 2148198 ?        Ssl    0:04 /usr/local/bin/plebble-wallet -d -c 0 -lp 16673 -pp 16673 -bp 16672 -e 20 -v 6 
</pre>

<p>Daemon control:</p>
<ul>
<li>Stop:
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>systemctl stop plebble-wallet 
</pre></li>
<li>Start:
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>systemctl start plebble-wallet 
</pre></li>
<li>Restart:
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>systemctl restart plebble-wallet 
</pre></li>
<li>Status:
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> root@plebble:~# </font>systemctl status plebble-wallet 
</pre></li>
</ul>

<h3>RPC client</h3>
<p>Command line. (Use under system user gov).</p>
<pre style="background-color:#000000; color:#ffffff;display: inline-block"">
<font color="#00FF00"> gov@plebble:~# </font>plebble-wallet -h 
plebble-wallet (rpc client). Channel 0. Copyright (C) 2017-2022 root1m3@plebble.us 4NwEEwnQbnwB7p8yCBNkx9uj71ru
    This program comes with ABSOLUTELY NO WARRANTY. For details type 'show w'.
    This is free software, and you are welcome to redistribute it under certain conditions. Type 'show c' for details.
    version: plebble-alpha-34_803807dc 8b16242a29b9b805c5bc4e5875b8447efdb00cd7 2022-06-07_08-03-18
    local time: 1656577571571648144 ns since 1/1/1970
    tx time shift: 15 seconds.
    Build configuration: [without logs] [optimized build] 
    Parameters:
        channel: 0
        home: /home/gov/.plebble
        this is an optimized build.
        logs: disabled.
        run rpc shell
          wallet daemon at: localhost:16673
          connect_for_recv 1
          stop_on_disconnection 1
Usage: plebble-wallet [options] [command]
Options:
    General parameters:                                                               .                                                                               
        -b                                                                                Basic mode.                                                                     
        -local                                                                            Instantiate a wallet instead of connecting to a daemon.. [false]                
        -home &lt;homedir&gt;                                                                   homedir. [/home/gov/.plebble]                                                   
        -d                                                                                Runs daemon (backend) instead of an rpc-client.. [16673]                        
        -e &lt;edges&gt;                                                                        Max num walletd neightbours. [8]                                                
        -v &lt;edges&gt;                                                                        Max num device neightbours. [4]                                                 
        -w &lt;workers&gt;                                                                      Threadpool size. [4]                                                            
        -lp &lt;port&gt;                                                                        Listening Port. (-d). [16673]                                                   
        -pp &lt;port&gt;                                                                        Published listening port.. [16673]                                              
        -pin &lt;PIN number&gt;                                                                 Connect using PIN.. [0]                                                         
        -c &lt;channel&gt;                                                                      System channel. [0]                                                             
        -dd &lt;downloads_dir&gt;                                                               Directory containing software updates blobs. [/home/gov/downloads]              
        -n                                                                                Omit field names in output. [true]                                              
        -nb                                                                               Don't show the banner..                                                         
        -bhost &lt;address&gt;                                                                  us-gov IP address. [localhost]                                                  
        -bp &lt;port&gt;                                                                        TCP port. [16672]                                                               
        -whost &lt;address&gt;                                                                  walletd address. [localhost]                                                    
        -wp &lt;port&gt;                                                                        walletd port. [16673]                                                           

Commands:
    misc:                                                                             ----------.                                                                     
        id                                                                                Print wallet id and exit..                                                      
        version                                                                           Print software version..                                                        

    cash: Query                                                                       ----------.                                                                     
        balance &lt;0|1|2&gt;                                                                   If detail is 0 it displays a collapsed view of accounts. 1 or 2 shows the breakd
                                                                                              own by account with the given level of detail.                                  
        recv_address                                                                      Shows a suitable address for receiving transfers.                               

    cash: Functions for transfering to &lt;dest account&gt;                                 ----------.                                                                     
        transfer &lt;dest account&gt; &lt;amount&gt; &lt;gas|coin&gt;                                       Source account/s will be choosen automatically.                                 
        transfer_from &lt;source account&gt; &lt;dest account&gt; &lt;amount&gt; &lt;gas|coin&gt;                 Selectable source account.                                                      

    cash: Two-step transactions                                                       ----------.                                                                     
        invoice &lt;recv account&gt; &lt;amount&gt; &lt;gas|coin&gt; [-reward &lt;award amount&gt; &lt;gas|coin&gt;]    Creates an invoice (incomplete transaction).                                    
        pay &lt;tx&gt;                                                                          Pay invoice. Complete the transaction and send it over.                         

    cash: Coin/token creation                                                         ----------.                                                                     
        set_supply &lt;address&gt; &lt;amount&gt;                                                     Set or reset the coin mint balance.                                             

    Transaction making                                                                ----------.                                                                     
        tx new                                                                            Creates an empty transaction.                                                   
        tx add_section &lt;tx&gt; &lt;token&gt;                                                       Creates a coin section in the given transaction.                                
        tx add_input &lt;tx&gt; &lt;section&gt; &lt;address&gt; &lt;amount&gt;                                    Creates an input in the specified section of the given transaction.             
        tx add_output &lt;tx&gt; &lt;section&gt; &lt;address&gt; &lt;amount&gt;                                   Creates an output in the specified section of the given transaction.            
        tx make_sigcode exec_time &lt;open|close&gt; [s &lt;section&gt; [i &lt;input&gt;]* [o &lt;output&gt;]* ]  .                                                                               
             [-]                                                                              
        tx decode_sigcode &lt;sigcode_b58&gt;                                                   .                                                                               
        tx ts &lt;tx&gt; &lt;secs in future&gt;                                                       Sets a new exec-time for the given transaction.                                 
        tx sign_input &lt;tx&gt; &lt;section&gt; &lt;input&gt; &lt;sigcode&gt; &lt;secret key&gt;                       Signs the given tx input.                                                       
        tx check &lt;tx&gt;                                                                     .                                                                               
        tx send &lt;tx&gt;                                                                      .                                                                               
        tx sign &lt;tx&gt; &lt;sigcodeb58&gt;                                                         .                                                                               
        tx decode &lt;evidenceB58&gt;                                                           .                                                                               

    keys:                                                                             ----------.                                                                     
        address new                                                                       Generates a new key-pair, adds the private key to the wallet and prints its asoc
                                                                                              iated address.                                                                  
        address add &lt;privkey&gt;                                                             Imports a given private key in the wallet.                                      
        list [show_priv_keys=0|1]                                                         Lists all keys. Default value for show_priv_keys is 0.                          
        gen_keys                                                                          Generates a key pair without adding them to the wallet..                        
        mine_public_key &lt;string&gt;                                                          Creates a vanity address containing the specified string.                       
        priv_key &lt;private key&gt;                                                            Gives information about the given private key.                                  
        pub_key &lt;public key&gt;                                                              Gives information about the given public key.                                   
        digest &lt;filename&gt;                                                                 Computes RIPEMD160+base58 to the content of the file..                          
        sign -f &lt;filename&gt; &lt;private key&gt;                                                  Sign file..                                                                     
        sign "&lt;message&gt;" &lt;private key&gt;                                                    Sign message.                                                                   
        verify -f &lt;filename&gt; &lt;pubkey&gt; &lt;signature&gt;                                         Verify signed file..                                                            
        verify "&lt;message&gt;" &lt;pubkey&gt; &lt;signature&gt;                                           Verify signed message.                                                          
        encrypt -f &lt;filename&gt; &lt;sender_private_key&gt; &lt;recipient_public_key&gt;                 Encrypts file..                                                                 
        encrypt "&lt;message&gt;" &lt;sender_private_key&gt; &lt;recipient_public_key&gt;                   Encrypts message.                                                               
        decrypt "&lt;b58&gt;" &lt;sender_public_key&gt; &lt;recipient_private_key&gt;                       Decrypts message.                                                               
        hash add &lt;hash1&gt; &lt;hash2&gt;                                                          Results in RIPEMD160(hash1, hash2)..                                            

    Device pairing:                                                                   ----------.                                                                     
        device_id                                                                         Show this device Id..                                                           
        pair &lt;pubkey&gt; [&lt;subhome&gt;|-] [&lt;name&gt;]                                              Authorize a device identified by its public key.                                
        unpair &lt;pubkey&gt;                                                                   Revoke authorization to the specified device.                                   
        prepair &lt;pin|auto&gt; [&lt;subhome&gt;|-] [&lt;name&gt;]                                         Pre-Authorize an unknown device identified by a pin number [1-65535].           
        unprepair &lt;pin&gt;                                                                   Revoke Pre-authorization to the specified pin.                                  
        list_devices                                                                      Prints the list of recognized devices, together with the list of recently unatho
                                                                                              rized attempts log.                                                             
        net_info                                                                          Prints contextual information and wallet type (root/guest).                     

    Public storage (on-chain):                                                        ----------.                                                                     
      key-value:
        store &lt;address&gt; -kv &lt;key&gt; &lt;value&gt;                                                 Store key-value pair in a funded account. cost: 1 per cycle.                    
        rm &lt;address&gt; -kv &lt;key&gt;                                                            Removes the key-value pair.                                                     
        search &lt;address&gt; &lt;word1|word2...&gt;                                                 Search key-value table.                                                         
      file:
        store &lt;address&gt; -f &lt;file&gt; [path]                                                  Store file in a funded account. cost: 1 per kibibyte per cycle.                 
        rm &lt;address&gt; -f &lt;hash&gt; &lt;path&gt;                                                     Removes a file. &lt;path&gt; example: /.                                              
        file &lt;hash&gt;                                                                       Retrieves file content.                                                         
        list &lt;address&gt; &lt;path&gt;                                                             List files starting with path.                                                  

    Private storage: (in-wallet)                                                      ----------.                                                                     
        timeseries list                                                                   List accounts holding timeseries..                                              
        timeseries &lt;address&gt; new                                                          Creates a new time series in the given address..                                
        timeseries &lt;address&gt; add [-f &lt;filename&gt;|-b58 &lt;b58&gt;|"text line"]                   Appends a new entry..                                                           
        timeseries &lt;address&gt; show [&lt;timemark&gt;]                                            List all events registered. Or obtain its content if a timemark is given..      

    Law/Compilance:                                                                   ----------.                                                                     
        compilance_report &lt;jurisdiction&gt; &lt;date-from&gt; &lt;date-to&gt;                            Produces a private report including personal, financial and ownership data that 
                                                                                              voluntarily could be submitted to regulators.                                   
    Trader:                                                                           ----------.                                                                     
    trade &lt;command&gt;                                                                   Access to trading functions.                                                    
        Trading help.
        Commands:
            [R2R Trades]                                                                      ----------.                                                                     
            qr                                                                                Display my QRs.                                                                 
            save_qr_bookmark &lt;name&gt; &lt;file&gt;                                                    Save to file (or stdout if no file) a bookmark of me by name.                   
            list_protocols                                                                    Lists available trading protocols.                                              
            start &lt;node_address&gt; &lt;protocol&gt; &lt;role&gt;                                            Initiates a new P2P private trade using endpoint.                               
            list                                                                              Lists all active trades.                                                        
            kill &lt;trade#&gt;                                                                     Terminates the specified trade.                                                 
            &lt;trade#&gt; [&lt;subcommand&gt;]                                                           Enter a shell for the specified trade.                                          
            reload &lt;full-path-doc&gt;                                                            Tell all active trades the specified file changed on disk.                      
            wallet                                                                            Exit trading shell going back to wallet shell.                                  

            [Service API]                                                                     ----------.                                                                     
            exec [&lt;cmd&gt;]                                                                      Exec command. Empty cmd for help.                                               

            [World]                                                                           ----------.                                                                     
            world                                                                             Lists remote wallets.                                                           
            bookmarks [file]                                                                  Print my bookmarks (or those in input file)..                                   
            bookmarks_append &lt;dstfile&gt; &lt;srcfile&gt;                                              Append bookmarks in srcfile into bookmarks file dstfile..                       
            bookmarks_rename &lt;file&gt; &lt;old-name&gt; &lt;new-name&gt;                                     Rename entry in bookmarks file..                                                
            save_bookmark &lt;name&gt; &lt;file&gt;                                                       Append to file (or stdout) a bookmark by name .                                 
            load_bookmark &lt;file&gt;                                                              Load .                                                                          


    Daemon control/monitoring:                                                        ----------.                                                                     
        s                                                                                 Show socket connections.                                                        
        gw                                                                                Show neighbours.                                                                
        allw                                                                              Show all wallets.                                                               
        regw &lt;ip address&gt;                                                                 Register this wallet reachable at the specified address..                       
        sync                                                                              Reload files.                                                                   
        reload_doc &lt;full-path-doc&gt;                                                        Invoke reload on all active trades in all wallets. Only via root wallet.        

    Net-dev control:                                                                  ----------.                                                                     
        patch_os &lt;script file&gt; &lt;skey&gt;                                                     System upgrade/maintenance. Requires governance key.                            
        (Account for updates: 4NwEEwnQbnwB7p8yCBNkx9uj71ru)

    Connectivity:                                                                     ----------.                                                                     
        ping                                                                              Endpoint check.                                                                 

    Software:                                                                         ----------.                                                                     
        version                                                                           Print version..                                                                 
        h|-h|help|-help|--help                                                            This help screen.   
</pre>
<p>Android app</p>
<a href="android_screenshots/Screenshot_2022-06-30-10-08-55-757_com.miui.home.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-30-10-08-55-757_com.miui.home.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-20-47-09-892_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-20-47-09-892_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-20-59-18-924_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-20-59-18-924_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-00-22-437_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-00-22-437_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-20-53-40-555_katlas.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-20-53-40-555_katlas.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-20-51-46-224_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-20-51-46-224_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-20-57-17-175_katlas.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-20-57-17-175_katlas.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-20-56-22-306_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-20-56-22-306_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-04-35-170_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-04-35-170_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-21-28-556_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-21-28-556_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-06-19-781_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-06-19-781_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-07-12-902_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-07-12-902_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-07-58-247_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-07-58-247_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-09-07-355_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-09-07-355_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/IMG_20220629_211014.jpg" target="_blank"><img src="android_screenshots/IMG_20220629_211014.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-18-47-013_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-18-47-013_plebble.trader.jpg" width="100"></a>
<a href="android_screenshots/IMG_20220629_212001.jpg" target="_blank"><img src="android_screenshots/IMG_20220629_212001.jpg" width="100"></a>
<a href="android_screenshots/IMG_20220629_212054.jpg" target="_blank"><img src="android_screenshots/IMG_20220629_212054.jpg" width="100"></a>
<a href="android_screenshots/Screenshot_2022-06-29-21-23-01-152_plebble.trader.jpg" target="_blank"><img src="android_screenshots/Screenshot_2022-06-29-21-23-01-152_plebble.trader.jpg" width="100"></a>

<ul>
<li>Install App:</li>
<p> From your android device point the internet browser to the url
    <pre>http://&lt;your-node-IP-address&gt;:16680/downloads/</pre>
    then navigate into directory <b>android/F5a1pov</b> and click on the apk file </p> 

<p> Alternatively you can get it from this node: <a href="http://plebble.net:16680/downloads/android/F5a1povb/" target="_blank">http://plebble.net:16680/downloads/android/F5a1povb/</a>
<li>Trouble-shooting</li>
<p>Enable 'Install apps from unknown parties'.</p>
<li>Connect your App with your node:</li>
</ul>

<h2>Related material</h2>
<p>Existing research papers</p>
<ul>
<li><a href="https://plebble.net/bitcoin.pdf">Bitcoin</a> - A Peer-to-Peer Electronic Cash System.</li>
<li><a href="http://lamport.azurewebsites.net/pubs/reaching.pdf">M. Pwase, R, Shostak, L. Lamport</a> - Reaching Agreement in the Presence of Faults.</li>
<li><a href="http://lamport.azurewebsites.net/pubs/time-clocks.pdf">Leslie Lamport</a> - Time, Clocks, and the Ordering of Events in a Distributed System</li>
<li><a href="https://pmg.csail.mit.edu/papers/osdi99.pdf">PBFT</a> - Practical Byzantine Fault Tolerance.</li>
<li><a href="https://arxiv.org/pdf/1804.01626.pdf">SBFT</a> - A Scalable and Decentralized Trust Infrastructure.</li>
<li><a href="https://rowstron.azurewebsites.net/PAST/pastry.pdf">Pastry</a> - Scalable, decentralized object location and routing for large-scale peer-to-peer systems.</li>
<li><a href="https://people.eecs.berkeley.edu/~sylvia/papers/cans.pdf">CAN</a> - A Scalable Content-Addressable Network.</li>
<li><a href="https://pdos.csail.mit.edu/papers/chord:sigcomm01/chord_sigcomm.pdf">Chord</a> - A Scalable Peer-to-peer Lookup Service for Internet Applications.</li>
<li><a href="https://pdos.csail.mit.edu/~strib/docs/tapestry/tapestry_jsac03.pdf">Tapestry</a> - A Resilient Global-Scale Overlay for Service Deployment.</li>
<li><a href="https://pdos.csail.mit.edu/~petar/papers/maymounkov-kademlia-lncs.pdf">Kademlia</a> - A peer-to-peer Information System Based on the XOR Metric.</li>
<li><a href="https://svn-archive.torproject.org/svn/projects/design-paper/tor-design.pdf">Tor</a> - The Second-Generation Onion Router.</li>
<li><a href="https://arxiv.org/pdf/2103.12112.pdf">Our-System</a> The quest for scaling BFT Consensus through Tree-Based Vote Aggregation.</li>
<!--
<li><a href="">User Manual</a>.</li>
-->
</ul>
</div>

<!-- ----------------------------------------- -->
<br/>
<button type="button" class="collapsible">+ Developer documentation.</button>
<div class="content">
<br/>
<h2>API</h2>
<table border="1" width="100%">
<tr>
<td valign="top">
<h2 align="center">Governance process.</h2>
<h3 align="center">(Public System)</h3>
<pre>
Interactions:
gov daemon <--> gov daemon
gov daemon <--> RPC client

</pre>
<p>API spec: <a href="https://github.com/root1m3/plebble/blob/main/core0/us/api/netsvc/gov" target="_blank">gov</a></p>

<h3>API implementation</h3>
<table border="1" width="100%">
<tr>
<td>C++</td><td>Java</td>
</tr>
<tr valign="top">
<td>
<p>service handlers - c++:</p>
<ul>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/gov/engine/peer_t.h" target="_blank">peer</a></li>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/gov/engine/daemon_t.h" target="_blank">daemon</a></li>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/gov/engine/rpc_peer_t.h" target="_blank">rpc_peer</a></li>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/gov/engine/rpc_daemon_t.h" target="_blank">rpc_daemon</a></li>
</ul>
<p>Binaries - c++:</p>
<ul>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/govx" target="_blank">console</a></li>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/gov" target="_blank">gov SDK</a></li>
</ul>
</td>

<td>
<p>service handlers - java:</p>
<ul>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/sdk/wallet/java/us/gov/engine/rpc_peer_t.java" target="_blank">rpc_peer</a></li>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/sdk/wallet/java/us/gov/engine/rpc_daemon_t.java" target="_blank">rpc_daemon</a></li>
</ul>

<p>Binaries - java:</p>
<ul>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/govj" target="_blank">console</a></li>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/sdk/gov/java" target="_blank">gov SDK</a></li>
</ul>
</td>
</tr>
</table>



</td>
<td>

<h2 align="center">Wallet process.</h2>
<h3 align="center">(Private System)</h3>
<pre>
Interactions:
wallet daemon <--> wallet daemon
wallet daemon <--> wallet RPC client
wallet daemon <--> gov daemon
</pre>
<p>API spec: <a href="https://github.com/root1m3/plebble/blob/main/core0/us/api/netsvc/gov" target="_blank">gov</a> <a href="https://github.com/root1m3/plebble/blob/main/core0/us/api/netsvc/wallet" target="_blank">wallet</a></p>

<h3>API implementation</h3>
<table border="1" width="100%">
<tr>
<td>C++</td><td>Java</td>
</tr>
<tr valign="top">
<td>
<p>service handlers - c++:</p>
<ul>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/wallet/engine/peer_t.h" target="_blank">peer</a></li>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/wallet/engine/daemon_t.h" target="_blank">daemon</a></li>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/wallet/engine/rpc_peer_t.h" target="_blank">rpc_peer</a></li>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/wallet/engine/rpc_daemon_t.h" target="_blank">rpc_daemon</a></li>
</ul>
<p>Binaries - c++:</p>
<ul>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/walletx" target="_blank">console</a></li>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/wallet" target="_blank">wallet SDK</a></li>
</ul>
</td>
<td>
<p>service handlers - java:</p>
<ul>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/sdk/wallet/java/us/gov/engine/rpc_peer_t.java" target="_blank">rpc_peer</a></li>
<li><a href="https://github.com/root1m3/plebble/blob/main/core0/us/sdk/wallet/java/us/gov/engine/rpc_daemon_t.java" target="_blank">rpc_daemon</a></li>
</ul>

<p>Binaries - java</p>
<ul>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/walletj" target="_blank">console</a></li>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/sdk/wallet/java" target="_blank">wallet SDK</a></li>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/android" target="_blank">android app</a></li>
</ul>
</td>
</tr>
</table>




</td>
</tr>
</table>

<h2>tests</h2>
<ul>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/test" target="_blank">core0 tests</a> Covering gov/+wallet(with bank protocol</li>
<li><a href="https://github.com/root1m3/plebble/tree/main/core1/us/trader/test" target="_blank">core1 tests</a> Covering the rest of an extensible list of r2r protocols</li>
</ul>

<h2>wallet plugins (aka role-2role/R2R protocols)</h2>
<ul>
<li><a href="https://github.com/root1m3/plebble/tree/main/core0/us/wallet/trader/r2r/w2w" target="_blank">bank</a>. Transfers/loans/banking services between 2 wallets.</li>
<li><a href="https://github.com/root1m3/plebble/tree/main/core1/us/trader/r2r/bid2ask" target="_blank">bid2ask</a>. Sell/buy goods and services between 2 wallets. </li>
</ul>

<h2>Continuous integration (CI)</h2>
<ul>
<li><a href="https://github.com/root1m3/plebble/tree/main/core1/bin" target="_blank">upgrade network sequence</a></li>
</ul>

<br/>
</div>
</br/>

<br/>
<button type="button" class="collapsible">+ Community.
<img src="community/reddit.png" width="20">
<img src="community/github.png" width="20">
<img src="community/twitter.png" width="20">
<img src="community/discord.png" width="20">
<img src="community/telegram.png" width="20">
<img src="community/signal.png" width="20">
</button>
<div class="content">
<p>Join plebble enthusiasts/contributors on</p>

<table width="100%">
<tr>
<td>
<img src="community/reddit.png" width="40"></td><td valign="middle">Plebble at <a href="https://www.reddit.com/r/plebble">Reddit r/plebble</a>.
</td>

<td>
<img src="community/github.png" width="40"></td><td valign="middle">Help the development!. Source code at <a href="https://github.com/root1m3/plebble">Github</a>.
</td>

<td>
<img src="community/twitter.png" width="40"></td><td valign="middle">Follow root1m3 on <a href="https://twitter.com/KatlasC">Twitter</a>.
</td>

</tr>
<tr>

<td>
<img src="community/discord.png" width="40"></td><td valign="middle">Join the <a href="https://discord.gg/jBwqcg8P67">discord</a> server.
</td>

<td>
<img src="community/telegram.png" width="40"></td><td valign="middle">Join the plebble <a href="https://t.me/joinchat/GJneFG6gALhUXrwO">Telegram</a> group.
</td>

<td>
<img src="community/signal.png" width="40"></td><td valign="middle">Join the plebble <a href="https://signal.group/#CjQKIN6_JG7cD-rpf17jwoe8fndbJ5-6GHbUjyVzkko71FF2EhAOecC_FiASSjzMxK48kIsn">Signal</a> group.
</td>

</tr>
</table>


</div>


<br/>

<br/>
<button type="button" class="collapsible">+ Inspirational books.</button>
<div class="content">
<ul>
<li>Mastering Bitcoin</li>
</ul>
</div>

<br/>
<button type="button" class="collapsible">+ Support Plebble.</button>

<div class="content" id="cdon">
<p>Please consider donating to help the development of this project.</p>
<p>Options:</p>
<ul>
<li>
<img src="community/patreon.png" width="40"/>Become a patron at <a href="https://www.patreon.com/plebble" target="_blank">Patreon</a>
</li>

<li>
<img src="community/github.png" width="40"/>Sponsor plebble at <a href="https://github.com/root1m3/plebble">Github</a>
</li>

<li>
<img src="btc.png" height="30"/>
Donate Bitcoin or Fiat via <a href="https://tallyco.in/s/88u9wf" target="_blank">tallycoin</a><br/>
</li>

<li>
<img src="btc.png" height="30"/>
Donate Bitcoin!.<br/>
Transfer some sats to address <a href="bitcoin:BC1QSR4HLJZCXHCUU6JMGCR2Y3HNWMDKQMDEDSMZFP">bitcoin:BC1QSR4HLJZCXHCUU6JMGCR2Y3HNWMDKQMDEDSMZFP</a>
<img src="./qr-of-public-key.png" id="qr" />
</li>



</ul>

</div>


<br/>
<br/>
<br/>

<table width="100%">
<tr>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
<td><img src="plebble_logo.png" height="50" alt="plebble logo"/></td>
</tr>
</table>

</pre>
</div>
<br/>

<pre>--</pre>
<a href="mailto:root1m3@plebble.us">root1m3@plebble.us</a>
<br/>
<pre>
Page generated on <?php
$date = new DateTime();
echo $date->format('Y-m-d H:i:s');
?>
</pre>

<script>
var coll = document.getElementsByClassName("collapsible");
var i;

for (i = 0; i < coll.length; i++) {
  coll[i].addEventListener("click", function() {
    this.classList.toggle("active");
    var content = this.nextElementSibling;
    if (content.style.maxHeight){
      content.style.maxHeight = null;
    } else {
      content.style.maxHeight = content.scrollHeight + "px";
    }
  });
}
</script>
</body>
</html>
