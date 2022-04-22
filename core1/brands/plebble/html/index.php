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

    $nn=shell_exec("cat /home/gov/pub/nodes.txt | grep \"^.* nodes.\" | awk '{ print $2 }'");
    $na=shell_exec("cat /home/gov/pub/accounts.txt | grep \"accounts:\" | awk '{ print $2 }'");
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
<h2>P2P distributed network of inexpensive nodes.</h2>
<h3>Blockchain multitoken platform designed for privacy and sovereignty.</h3>

<font size="-1" color="#FFFFFF">
<table bgcolor="#000000" width="90%" border="0" cellspacing="10">
<!--
<tr>
<td colspan="3"><font size="-1">URL: <a href="pub/dindex.html">dhttp://dindex.html</a></font></font></td>
</tr>
-->
<tr>
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
</td>
<td width="100%" nowrap>
<a class="hdl" href="pub/accounts.txt">Ledger</a>: <b><?=$na?></b>
</td>
</tr>
<tr>
<td colspan="3" nowrap>Software version: <b><?=$swver?></b> [<a class="hdl" href="pub/release_notes.txt">release notes</a>]</td>
</td>
</tr>
</table>

</center>


</font>

<center>
<font face="courier" size="+1">
<p>WEB3: Plumbing the future of internet.</p>
<p>We seek decentralization.</p>
<p>We are not pursuing a global economy driven by 1 'central' token,</p>
<p>    but a real distributed economy driven by billions of tokens.</p>
</p>
</font>
</center>

<table cellspacing="10">
<tr>
<td bgcolor="#magenta">
<pre>
The network has recently been upgraded. If you own a node and for some reason is not working anymore follow this steps:
1- Login as user gov.
   This tipically is done:
   using a linux system: ssh gov@<local_IP_of_your_node> -p <port>
                        e.g. ssh -p16671 gov@192.168.1.12
   from windows: use the program <a target="_blank" href="https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html">putty</a>
	connect using your node address and ssh port (default port is 16671)
2.- execute:
   wget https://plebble.net/downloads/plebble_setup
   wget https://plebble.net/downloads/2TqABU2qt1kt1vE9tURb1PosLuUK
   chmod +x plebble_setup
   sudo ./plebble_setup --blob 2TqABU2qt1kt1vE9tURb1PosLuUK

This should upgrade your plebble node to the latest version.

check your balance to test it is alright. Execute:
   gov version
	(it should respond "plebble-alpha-29.14_a1527f.d1b79b a1527f6ac6913ebe0ec9be85b241a04d081bbbe1 2022-02-12_00-40-48")
   wallet balance

This same procedure can be used to start a new node from a new debian based operating system (debian, ubuntu, raspbian).
</pre>
</td>
</tr>
<tr>
<td id="comparisons"><b>Comparisons</b>:
 <ul>
  <li>other L1 crypto-currency systems: Bitcoin BTC, Bitcoin-cash BCH, Ethereum</li>
  <li>other exchanges: Binance, etc </li>
 </ul>
</td>
</tr>
<tr>
<td id="guides"><b>Plebble. Guides</b>:
 <ul>
  <li><b>Raspberri Pi:</b></li>
  <ul>
   <li>hardware <a href="shopping_list.txt">shopping_list</a></li>
   <li><a href="pub/install_instructions/node_setup_instructions_linux.txt" title="Node setup instructions (using Linux)">Node setup 3-step guide</a> for GNU/Linux users.</li>
   <li><a href="pub/install_instructions/node_setup_instructions_windows.txt" title="Node setup instructions (using Windows)">Node setup 3-step guide</a> for <a href="https://en.wikipedia.org/wiki/Microsoft_Windows" target="_blank">Windows</a> users.</li>
  </ul>
  <li><b>VM (Debian/Ubuntu):</b></li>
  <ul>
   <li>Node bootstrap script: wget <a href="https://plebble.net/setup">https://plebble.net/setup</a></li>
  </ul>
  <li>A conceptual twist of the Bitcoin white paper: <a href="http://plebble.net/bitcoin.pdf">Bitcoin - A Peer-to-Peer Electronic Cash System.</a></li>
    <!-- <li>Docs: Plebble a Zero-MEV system <a href="ZeroMEV.pdf">ZeroMEV.pdf</a></li> -->
  <li>Technical documentation:
    <ul>
      <li>User's manual: [To be produced]</li>
      <li>Technical documentation: [To be produced]</li>
      <li>Source code documentation: [<a href="http://plebble.net/doxygen">doxygen</a>]</li>
    </ul>
  </li>
 </ul>
</td>
</tr>

<tr>
<td><b>Download</b>:
<ul>
<li>Node+non-custodial wallet:
<ul>
<li><a href="plebble-os_raspi_alpha-11_b4b8240eb38bcc7ea270f4c6a1d9f248adeb3567.zip" title="Download image for Raspberry Pi (Zero, 3B+, 4)" alt="raspi OS image">Raspberry Pi (Zero, 3B+, 4)</a>. Plebble-OS</li>
<li>
VM (Debian 10/buster) Node bootstrapping 3-liner:<br/>
<pre>
wget https://plebble.net/setup
chmod +x setup
./setup
</pre>
Plebble lowers the costs of running a VM node (hosted virtual machine) down to just $5/month. [debian 10 droplet at <a href="https://www.digitalocean.com/">Digital Ocean</a>].<br/></li>
</ul>
</li>
<li>Onboarding fast lane: Custodial wallet (it doesn't need to setup a node, it only requires installing a mobile app) [soon]</li>
</ul>
</td>
</tr>
<tr>
<td><b>Community</b>:
<ul>

<table>
<tr>
<td>
<li>
<img src="community/twitter.png" width="40"></td><td valign="middle">Follow root1m3 on <a href="https://twitter.com/KatlasC">Twitter</a>.
</li>
</td>
</tr>

<tr>
<td>
<li>
<img src="community/signal.png" width="40"></td><td valign="middle">Join our plebble <a href="https://signal.group/#CjQKIN6_JG7cD-rpf17jwoe8fndbJ5-6GHbUjyVzkko71FF2EhAOecC_FiASSjzMxK48kIsn">Signal</a> group.
</li>
</td>
</tr>

<tr>
<td>
<li>
<img src="community/telegram.png" width="40"></td><td valign="middle">Join our plebble <a href="https://t.me/joinchat/GJneFG6gALhUXrwO">Telegram</a> group.
</li>
</td>
</tr>

<tr>
<td>
<li>
<img src="community/github.png" width="40"></td><td valign="middle">Plebble at <a href="https://github.com/root1m3/plebble">Github</a>.
</li>
</td>
</tr>

<tr>
<td>
<li>
<img src="community/patreon.png" width="40"></td><td valign="middle">Plebble at <a href="https://www.patreon.com/plebble">Patreon</a>.
</li>
</td>
</tr>

</table>
</ul>
</td>
</tr>

<tr>
<td><b>Join us</b>:
<ul>
<li>Run a node, make sure it runs on channel 0. Let's make the network a bit bigger. A consensus-split (meaning the network splits into two halves) shall happen transparently when number of nodes N > 1000.</li>
</ul>
</td>
</tr>

<tr>
<td><b>Review the source code:</b></b>:
<ul>
<li>git: <a href="https://github.com/root1m3/plebble" target="_blank">https://github.com/root1m3/plebble</a></li>
</ul>
</td>
</tr>

<tr>
<td><a name="donate"><b>Donate: push us forward!</b></a>:
<p>
Please consider donating. We need backers in order to support the continuous development of this project, which can be seen as a distributed company where users are all shareholders. Unfortunately VC profit-seeking algorithms can't see any profit for them and hence they don't approach. Besides, it is sort of a success for us ( :-P ), since it means maximum alignment with a design-mantra: to be the profit machine of choice for its users. Plebble is a free tool for helping privacy-seeking, self-sovereign users in trading their portfolio of valuables, while protecting them from incuring in any issue with the Law. Laws are configurable by user at node scope, e.g. North Pole Law. Help us reach there. Thanks.
<table>
<tr>
<td colspan="2"><a href="bitcoin:BC1QSR4HLJZCXHCUU6JMGCR2Y3HNWMDKQMDEDSMZFP">bitcoin:BC1QSR4HLJZCXHCUU6JMGCR2Y3HNWMDKQMDEDSMZFP</a></td>
</tr>
<tr>
<td>
<img src="btc.png" height="30">
</td>
<td>
<a href="#donate"><span id="qrl" onclick="document.getElementById('qr').style.display='inline'; document.getElementById('qrl').style.display='none';">Show QR</span> <img style="display: none" src="./qr-of-public-key.png" id="qr" /></a>
</td>
</tr>
</table>

</td>
</tr>

</table>
<hr/>

<h3><a href="https://www.urbandictionary.com/define.php?term=tl%3Bdr" target="_blank">tl;dr</a></h3>
<p>Plebble is a L1 (Level 1) multi-coin cryptocurrency system. If you already like Bitcoin but you dont like Nakamoto Consensus (Proof of Work, Proof of Stake or any other variant)
because they are far from being sustainable or not egalitarian, and you also advocate for privacy and wallets automation with P2P trading capabilities,
if you like the futuristic idea of an automated state based on distributed power where the invidual is sovereign and shareholders of a system that, provided it becomes mainstream, could deliver a 'universal salary' from the profits made by mass-comsumption services like cash transfers, then Plebble is a project you should look into in more detail and eventually decide to run an inexpensive node at home.</p>
<p>Nodes are paid in native token 'gas' for validating transactions and for securing the network against malicious attacks.<p>

<h3>Check it out!</h3>
<p>It is free and permissionless. Anyone can join.</p>

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
<a href="mailto:root1m3@plebble.us">root1m3@plebble.us</a>
<br/>
<pre>
Page generated on <?php
$date = new DateTime();
echo $date->format('Y-m-d H:i:s');
?>
</pre>

</body>
</html>
