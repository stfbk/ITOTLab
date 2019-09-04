To configure a VPC between the AWS RDS instance and an AWS EC2 VM, please refer here: https://docs.aws.amazon.com/en_us/AmazonRDS/latest/UserGuide/CHAP_Tutorials.WebServerDB.CreateVPC.html. To configure a MySQL instance in AWS RDS, open the AWS web management console, open the RDS service and click on *Create Database*. Then:

1. In the *Select engine* view, select *MySQL* and *Only enable options eligible for RDS Free Usage Tier* to ensure no payments are charged if using the 12-months free tier.
2. In the Specify DB details view, select the version (e.g., *MySQL 5.7.x* - current version is MySQL 5.7.23).
2.1. Specify a DB instance identifier (e.g. *rds-mysql-iotpolicydb*), a master username and its password.
2.2. Leave to default values: *License model* (set to general-public-license), *DB instance class* (db.t2.micro — 1 vCPU, 1 GiB RAM), *Allocated storage* (20 GiB) and the *Storage type*.
3. In the Configure advanced settings view, set the database name (e.g. innodb) and the *Backup window* (e.g., to 1h and starting at midnight)
4. Enable *Enhanced monitoring*, with *Monitoring Role* to default and *Granularity* to 60s
(the option ``I authorize RDS to create the IAM role rds-monitoring-role''' is automatically enabled)
5. Log everything to to Amazon CloudWatch (*Log exports* view)
6. Set *Maintenance window* (e.g., to 1h, starting on sunday at 2AM)
7. Leave to default values: Virtual Private Cloud (*Default VPC(..)* or select the one created during the AWS VPC tutorial), Availability zone (no preference), Create new VPC security group (No if following the AWS VPC tutorial), Port (3306), DB parameter group and Option group (default.mysql5.7), Backup retention period (e.g., 7 days), Copy tags to snapshots (enabled), Maintenance (Enable auto minor version upgrade), Deletion protection(enabled) and public accessibility (no). If creating a new security group, in the *details pane* of the *Instances* tab, select the security group and modify the inbound rule to allow *Your IP* (on the port 3306).