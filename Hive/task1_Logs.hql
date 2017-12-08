ADD jar /opt/cloudera/parcels/CDH/lib/hive/lib/hive-serde.jar;
ADD jar /opt/cloudera/parcels/CDH/lib/hive/lib/hive-contrib.jar;

USE mcs2017012_test;
DROP TABLE IF EXISTS Logs;

CREATE EXTERNAL TABLE Logs (
        ip        STRING,
        time      INT,
        http_ask  STRING,
        page_size INT,
        status    INT,
        app_info  STRING
)


ROW FORMAT SERDE 'org.apache.hadoop.hive.serde2.RegexSerDe'
WITH SERDEPROPERTIES (
        "input.regex" = '^(\\S*)\\t\\t\\t(\\d{8})\\S*\\t(\\S*)\\t(\\S*)\\t(\\d{3})\\t(\\S*)\\s.*$'
)

STORED AS TEXTFILE
location '/data/user_logs/user_logs_M';
