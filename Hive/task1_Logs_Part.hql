ADD JAR /opt/cloudera/parcels/CDH/lib/hive/lib/hive-contrib.jar;
ADD jar /opt/cloudera/parcels/CDH/lib/hive/lib/hive-serde.jar;
SET hive.exec.dynamic.partition.mode=nonstrict;
USE mcs2017012_test;

DROP TABLE IF EXISTS Logs_Part2;

CREATE EXTERNAL TABLE Logs_Part2 (
        ip        STRING,
        --time_new  INT,
        http_ask  STRING,
        page_size INT,
        status    INT,
        app_info  STRING
)

PARTITIONED BY (time INT)
STORED AS TEXTFILE;
INSERT OVERWRITE TABLE Logs_Part2 PARTITION (time)
SELECT * FROM Logs;

