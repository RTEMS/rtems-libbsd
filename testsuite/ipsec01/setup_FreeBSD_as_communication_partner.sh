#!/bin/sh -x

if ! which racoon >/dev/null 2>&1
then
	echo "You have to install security/ipsec-tools from ports collection!"
	exit 1
fi

IPSEC_REM_INT="10.10.1.1"
IPSEC_REM_NET="10.10.1.0/24"
IPSEC_REM_EXT="192.168.10.1"
IPSEC_LOC_INT="172.24.0.1"
IPSEC_LOC_NET="172.24.0.0/24"
IPSEC_LOC_EXT="192.168.10.10"
RACOON_PSK_FILE="/etc/racoon_psk.txt"
RACOON_CONFIG_FILE="/etc/racoon.conf"
SETKEY_CONF="/etc/setkey.conf"
GIF="gif0"

ifconfig $GIF create
ifconfig $GIF $IPSEC_LOC_INT $IPSEC_REM_INT
ifconfig $GIF tunnel $IPSEC_LOC_EXT $IPSEC_REM_EXT
route add $IPSEC_REM_NET $IPSEC_REM_INT

cat <<EOF > $RACOON_CONFIG_FILE
path   pre_shared_key "$RACOON_PSK_FILE";
log    debug;

padding	# options are not to be changed
{
        maximum_length  20;
        randomize       off;
        strict_check    off;
        exclusive_tail  off;
}

listen	# address [port] that racoon will listen on
{
        isakmp          $IPSEC_LOC_EXT [500];
}

remote $IPSEC_REM_EXT [500]
{
       exchange_mode                   main;
       my_identifier                   address $IPSEC_LOC_EXT;
       peers_identifier                address $IPSEC_REM_EXT;
       proposal_check                  obey;

       proposal {
               encryption_algorithm    3des;
               hash_algorithm          md5;
               authentication_method   pre_shared_key;
               lifetime time           3600 sec;
               dh_group                2;
       }
}

sainfo (address $IPSEC_LOC_NET any address $IPSEC_REM_NET any)
{
       pfs_group                       2;
       lifetime                        time 28800 sec;
       encryption_algorithm            3des;
       authentication_algorithm        hmac_md5;
       compression_algorithm           deflate;
}
EOF

cat <<EOF > $RACOON_PSK_FILE
$IPSEC_REM_EXT mysecretkey
EOF

chmod 600 $RACOON_PSK_FILE

cat <<EOF > $SETKEY_CONF
flush;
spdflush;
spdadd $IPSEC_LOC_NET $IPSEC_REM_NET any -P out ipsec esp/tunnel/$IPSEC_LOC_EXT-$IPSEC_REM_EXT/use;
spdadd $IPSEC_REM_NET $IPSEC_LOC_NET any -P in  ipsec esp/tunnel/$IPSEC_REM_EXT-$IPSEC_LOC_EXT/use;
EOF

setkey -f /etc/setkey.conf
racoon -F -f /etc/racoon.conf
