# plebble
An open source P2P digital currency

Plebble is a currency, a protocol, and a software that enables

    Instant peer to peer transactions
    Worldwide payments
    Low processing fees
    And much more!

Plebble uses peer to peer technology to operate with no central authority; managing transactions and issuing gas are carried out collectively by the network.
Through many of its unique properties, plebble allows exciting uses that could not be covered by any previous payment systems.

The software is a community-driven free open source project, released under the free GPL license.

# Introduction
This codebase produces the binaries run by nopdes in the plebble network.

Plebble is pretty much designed along cypherpunk lines inpired by Bitcoin, centered on values like self-sovereignty, privacy, anonymity, fairness, borderless, inclusive, distributed control and censorship resistance.

Plebble is a peer-to-peer permissionless network, which means that anyone can join.

Nodes run a public blockchain protocol with a nobel cooperative consensus mechanism where user wallets are all paid for their work (transaction verification work) and their role in securing the network against malicious attacks.

The network increases its resiliency with the number of nodes.

Sybils (spawned nodes) are controlled by IPv4 address preventing an attacker from accumulating
voting power without paying the cost of acquiring different IPv4 addresses, which are 'expensive' and scarce.

Major features:
  * creating and transfering gas (the coin controlling the behaviour (cryptoeconomics) of the system).
  * creating and transfering user-created coins.
  * Store data in accounts (Digital assets, NFC, ..)
  * Privacy smart wallet powered by R2R protocols. Role-to-role protocols enable powerful trading capabilities between online wallets.
  * Console interfaces and an android app interacting exclusively with your node.

Contributions in any form are welcome.

# source code stats reported by the program sloccount
Totals grouped by language (dominant language first):
cpp:          68848 (63.79%)
java:         24113 (22.34%)
sh:            8117 (7.52%)
xml:           5029 (4.66%)
cs:            1761 (1.63%)
php:             56 (0.05%)

Total Physical Source Lines of Code (SLOC) = 107,924
(Excluded comments and blank lines)


# DEV INSTRUCTIONS

BUILD STEPS FOR BEGINNERS
=========================

bin/configure
bin/make -j8
bin/run_tests

FILES - GENERAL DESCRIPTION
===========================

Core0 [core0/] Public governance network - Private trading network
   Public System
     BACKEND
       * [us/gov/] - kernel 
     HMI - Human-Machine Interfaces:
       * [us/govx/] - Console RPC client.
   Private System
     BACKEND
       * [us/wallet/] - kernel
       * [sdk] - RPC client libraries in various languages (java, c#, ...)
     HMI - Human-Machine Interfaces:
       * [us/walletx/] - Console RPC client. (written in C++)
       * [us/walletj/] - Console RPC client. (written in Java)
       * [us/android/] - Compatible with Android Studio

Core1 [core1/] devops toolchain
   -plebble - branding particularities. UX, Look&Feel, names, colours, ...
       * [brands/]
   -r2r -Role-to-Role protocols
       * [r2r/]
   -cbs -continous build script
       * [bin/cbs]
