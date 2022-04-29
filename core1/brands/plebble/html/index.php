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
//	echo "argc and argv disabled\n";
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
	</style>
  </head>
<body style="min-width:678px;">

<center>
<table bgcolor="#000000" cellspacing="25" width="90%">
<tr><td align="center">
<a href="https://plebble.us">
<img src="plebble_logo.png" height="100" alt="plebble logo"/><img src="header.png" alt="plebble" height="100"/></td>
</a>
</tr>
</table>
<h2>P2P ecosystem</h2>
<h3>Featuring cooperative consensus among untrusted nodes.</h3>
<p>free open source software</p>

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
<button type="button" class="collapsible">Introduction</button>
<div class="content">
<p>Plebble is a L1 (Level 1) multi-coin cryptocurrency system.</p>
<p>You can think of it as a heavy remake of <a href="https://bitcoin.org" target="_blank">Bitcoin</a> built 100% from scrach with the following differences:</p>
<ul>
 <li>Miners do not exist: All nodes participate in the production of new blocks, thanks to a novel algorithm called <a href="https://www.reddit.com/r/plebble/comments/u479f2/cooperative_consensus/" target="_blank">cooperative consensus</a>.</li>
 <li>All nodes compute the same block, and <b>all</b> are receiving their share of the profit from the very first minute. [<a href="https://www.reddit.com/r/plebble/comments/u8rt8e/how_to_implement_universal_salary/" target="_blank">Universal Salary</a>]</li>
 <li>Aside from running a single Coin with fixed cryptoeconomics rules, users are able to create <b>their coins</b> with their own inlation/deflation laws.</li>
 <li>Instead of having a pointless hardcap, Plebble.GAS cryptoeconomics algorithm for the supply law is dynamic, <b>printing and/or burning money algorithmically</b> in order to optimize <a href="" target="_blank">social efficiency</a>.</li>
</ul>
<p>...and the following similarities:</p>
<ul>
<li>It is free, borderless and permissionless. Anyone can join and participate.</li>
<li>Not tied to any profit seeking company.</li>
</ul>
</div>


<br/>
<button type="button" class="collapsible">Run a node</button>
<div class="content">

<h3>Hardware requirements.</h3>
<p>The aim of plebble is to achieve the lowest entry barrier for running a node by means of witing efficient code and algorithms.</p>
<ul>
<li><b>Raspberry Pi</b></li>
  <img src="pi4.png" alt="pi4" width="200"/>
  <p>A practical and stable option, meeting the highest standards on privacy and network distribution.</p>
  <p>hardware <a href="shopping_list.txt">shopping_list</a></p>
  <p>Install <a href="https://www.raspberrypi.com/software/" target="_blank">raspberry pi OS</a> (formerly known as raspbian) [<a href="https://www.youtube.com/watch?v=ntaXWS8Lk34" target="_blank">short video</a>]</p>
  <p>In step 2 Choose Operating System "Raspberry PI OS (32-bits)" in Raspberry Pi Imager</p>
<li><b>VM</b></li>
  <img src="cloud.png" alt="cloud" width="200"/>
<p>
Privacy seeking individuals don't like services in the cloud and will run a node in their own hardware.<br/>
However, people who trust 3rd parties can find it easier to run a plebble node in a custodial/hosted VM (Virtual machine) e.g. hiring a debian 11 droplet at <a href="https://www.digitalocean.com/" target="_blank">Digital Ocean</a> for $5/mo. Watch the easy process (implying a few clicks and pasting a command in the console) on <a href="https://www.youtube.com/watch?v=5drAx5EYK7s" target="_blank">this recording</a>.
</p>
</ul>
<br/>
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
<button type="button" class="collapsible">Documentation</button>
<div class="content">
<p>Related existing research papers</p>
<ul>
<li><a href="https://plebble.net/bitcoin.pdf">Bitcoin</a> - A Peer-to-Peer Electronic Cash System.</li>
<li><a href="https://pmg.csail.mit.edu/papers/osdi99.pdf">PBFT</a> - Practical Byzantine Fault Tolerance.</li>
<li><a href="https://arxiv.org/pdf/1804.01626.pdf">SBFT</a> - A Scalable and Decentralized Trust Infrastructure.</li>
<li><a href="https://rowstron.azurewebsites.net/PAST/pastry.pdf">Pastry</a> - Scalable, decentralized object location and routing for large-scale peer-to-peer systems.</li>
<li><a href="https://people.eecs.berkeley.edu/~sylvia/papers/cans.pdf">CAN</a> - A Scalable Content-Addressable Network.</li>
<li><a href="https://pdos.csail.mit.edu/papers/chord:sigcomm01/chord_sigcomm.pdf">Chord</a> - A Scalable Peer-to-peer Lookup Service for Internet Applications.</li>
<li><a href="https://pdos.csail.mit.edu/~strib/docs/tapestry/tapestry_jsac03.pdf">Tapestry</a> - A Resilient Global-Scale Overlay for Service Deployment.</li>
<li><a href="https://pdos.csail.mit.edu/~petar/papers/maymounkov-kademlia-lncs.pdf">Kademlia</a> - A peer-to-peer Information System Based on the XOR Metric.</li>
<li><a href="https://svn-archive.torproject.org/svn/projects/design-paper/tor-design.pdf">Tor</a> - The Second-Generation Onion Router.</li>
<!--
<li><a href="">User Manual</a>.</li>
-->
</ul>
</div>


<br/>
<button type="button" class="collapsible">Community</button>
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

<td>
<img src="community/discord.png" width="40"></td><td valign="middle">Join the <a href="https://discord.gg/Netgdpgy">discord</a> server.
</td>

</tr>
<tr>

<td>
<img src="community/signal.png" width="40"></td><td valign="middle">Join the plebble <a href="https://signal.group/#CjQKIN6_JG7cD-rpf17jwoe8fndbJ5-6GHbUjyVzkko71FF2EhAOecC_FiASSjzMxK48kIsn">Signal</a> group.
</td>

<td>
<img src="community/telegram.png" width="40"></td><td valign="middle">Join the plebble <a href="https://t.me/joinchat/GJneFG6gALhUXrwO">Telegram</a> group.
</td>

<td>
<img src="community/patreon.png" width="40"></td><td valign="middle">Support Plebble development at <a href="https://www.patreon.com/plebble">Patreon</a>.
</td>

<td>
<img src="community/patreon.png" width="40"></td><td valign="middle">Support Plebble development at <a href="https://www.patreon.com/plebble">Patreon</a>.
</td>
</tr>
</table>


</div>


<br/>

<button type="button" class="collapsible">Support Plebble</button>

<div class="content" id="cdon">
<p>Please consider donating to help the development of this project.</p>

<table>
<tr>
<td colspan="2"><a href="bitcoin:BC1QSR4HLJZCXHCUU6JMGCR2Y3HNWMDKQMDEDSMZFP">bitcoin:BC1QSR4HLJZCXHCUU6JMGCR2Y3HNWMDKQMDEDSMZFP</a></td>
</tr>
<tr>
<td>
<img src="btc.png" height="30">
</td>
<td>
<!-- <a href="#donate"><span id="qrl" onclick="document.getElementById('qr').style.display='inline'; document.getElementById('qrl').style.display='none';">Show QR</span><img style="display: none" src="./qr-of-public-key.png" id="qr" />
</a> -->
<img src="./qr-of-public-key.png" id="qr" />
</td>
</tr>
</table>

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
