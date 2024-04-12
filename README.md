# pg_linux_stats

`pg_linux_stats` is a set of PostgreSQL functions that provides information similar to the Linux commands vmstat, iostat, netstat and mpstat.

It was developed to simplify OS statistics monitoring without requiring the use of heavyweight solutions like Zabbix and Prometheus.

### Limitation

`pg_linux_stats` can only run on Linux.

## Installation

You can install it to do the usual way shown below.

```
$ tar xvfj postgresql-16.0.tar.bz2
$ cd postgresql-16.0/contrib
$ git clone https://github.com/s-hironobu/pg_linux_stats.git
$ cd pg_linux_stats
$ make && make install
```

You must add the line shown below in your postgresql.conf.

```
shared_preload_libraries = 'pg_linux_stats'
```

After starting your server, you must issue `CREATE EXTENSION` statement shown below.

```
postgres=# CREATE EXTENSION pg_linux_stats;
```


The `sysstat` module is required for `mpstat` command  on some Linux distributions.

Installation:

```
$ sudo apt install sysstat
```


## How to use

### pg_vmstat

```
testdb=# select * from pg_vmstat();
 r | b | swpd |  free  | buff  | cache  | si | so | bi | bo | interrupts | cs | us | sy | id  | wa | st
---+---+------+--------+-------+--------+----+----+----+----+------------+----+----+----+-----+----+----
 0 | 0 |    0 | 191608 | 30304 | 500916 |  0 |  0 | 14 | 14 |          0 |  5 |  0 |  0 | 100 |  0 |  0
(1 row)
```

### pg_netstat

```
testdb=# select * from pg_netstat();
 iface  |  mtu  |  rx_ok  | rx_err | rx_drp | rx_ovr |  tx_ok  | tx_err | tx_drp | tx_ovr | flg
--------+-------+---------+--------+--------+--------+---------+--------+--------+--------+------
 enp0s3 |  1500 | 1187590 |      0 |      0 |      0 |  221529 |      0 |      0 |      0 | BMRU
 enp0s8 |  1500 |   20223 |      0 |      0 |      0 |     895 |      0 |      0 |      0 | BMRU
 enp0s9 |  1500 | 2825614 |      0 |      0 |      0 | 1911326 |      0 |      0 |      0 | BMRU
 lo     | 65536 |    2835 |      0 |      0 |      0 |    2835 |      0 |      0 |      0 | LRU
(4 rows)
```

### pg_iostat

```
testdb=# select * from pg_iostat();
 device | tps  | kb_read_s | kb_wrtn_s | kb_dscd_s | kb_read  | kb_wrtn  | kb_dscd
--------+------+-----------+-----------+-----------+----------+----------+---------
 loop0  |    0 |         0 |         0 |         0 |      397 |        0 |       0
 loop1  |    0 |      0.02 |         0 |         0 |    48131 |        0 |       0
 loop2  | 0.01 |      0.23 |         0 |         0 |   716922 |        0 |       0
 loop3  |    0 |         0 |         0 |         0 |     1173 |        0 |       0
 loop4  | 0.01 |      0.15 |         0 |         0 |   459545 |        0 |       0
 loop5  |    0 |       0.1 |         0 |         0 |   308971 |        0 |       0
 loop6  |    0 |         0 |         0 |         0 |     1182 |        0 |       0
 loop7  |    0 |         0 |         0 |         0 |       97 |        0 |       0
 loop8  |    0 |         0 |         0 |         0 |      111 |        0 |       0
 sda    | 1.74 |     27.74 |     27.32 |         0 | 87167977 | 85825845 |       0
 sdb    |    0 |         0 |         0 |         0 |     3226 |        0 |       0
(11 rows)
```

### pg_mpstat

```
testdb=# select * from pg_mpstat();
 cpu | usr  | nice | sys  | iowait | irq | soft | steal | guest | gnice | idle
-----+------+------+------+--------+-----+------+-------+-------+-------+-------
 all | 0.15 | 0.01 |  0.1 |   0.01 |   0 | 0.02 |     0 |     0 |     0 | 99.72
 0   | 0.16 |    0 | 0.11 |   0.01 |   0 | 0.01 |     0 |     0 |     0 | 99.71
 1   | 0.14 | 0.01 | 0.09 |      0 |   0 | 0.03 |     0 |     0 |     0 | 99.73
(3 rows)
```
### pg_free

```
testdb=# select * from pg_free();
 total  |  used  |  free  | shared |  buff  | available | swap_total | swap_used | swap_free
--------+--------+--------+--------+--------+-----------+------------+-----------+-----------
 980320 | 257292 | 191608 |  48124 | 531420 |    510420 |          0 |         0 |         0
(1 row)
```

## Change Log
 - 28 Mar, 2024: Version 1.0 Released.
