FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libpcap-dev \
    libssl-dev \
    libpq-dev \
    libsqlite3-dev \
    libgeoip-dev \
    git \
    pkg-config \
    # Runtime deps
    postgresql-client \
    psmisc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/capanalysis

# Create structure expected by Makefile
RUN mkdir -p ../xplico/modules && \
    touch ../xplico/xplico

# Copy source
COPY . /opt/capanalysis/src/

WORKDIR /opt/capanalysis/src

# Create dummy modules to satisfy Makefile copy loop
RUN for mod in dis_dns_ca.so dis_tcp_ca.so dis_udp_ca.so disp_none.so disp_capostgres.so cap_ca.so cap_pcap.so dis_pcapf.so dis_pol.so dis_eth.so dis_pppoe.so dis_ppp.so dis_ip_nocheck.so dis_ipv6.so dis_tcp_soft.so dis_tcp_soft_nocheck.so dis_udp_nocheck.so dis_vlan.so dis_sll.so dis_ieee80211.so dis_llc.so dis_ppi.so dis_prism.so dis_ipsec.so dis_ipsec_ca.so dis_null.so dis_radiotap.so dis_mpls.so dis_l2tp.so dis_chdlc.so dis_gre.so; do touch ../xplico/modules/$mod; done

# Build
# We only build capanalysis (the daemon) and avoid the recursive make to 'pcapseek'
# unless we confirm we have that source too.
# Checking file list: pcapseek/ exists.
# But 'cpxplico' target requires actual xplico modules.
# We will just build the binary.
RUN make capanalysis

# Setup directories
RUN mkdir -p /opt/capanalysis/bin/modules \
    && cp capanalysis /opt/capanalysis/bin/ \
    && cp -a config /opt/capanalysis/cfg \
    && mkdir -p /opt/capanalysis/log \
    && mkdir -p /opt/capanalysis/tmp \
    && mkdir -p /opt/capanalysis/db/postgresql

# Entrypoint
WORKDIR /opt/capanalysis
CMD ["./bin/capanalysis", "-b", "postgres", "-c", "cfg/canalysis.cfg"]
