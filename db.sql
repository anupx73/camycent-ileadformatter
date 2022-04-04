/*

suspended  activate disable user

Lead Count

Country Assignment 

Payment History

Payment Reminder

Queue Rated Bill 

Billing Cycle 10th of Every Month

Send Reminder on 8th of Every Month

Email Admin about Dues on 15th of Every Month

Email Suspention Warning on 16th of Every Month

Suspend Account ( Manually on 17th of Every Month )

Diactivate Account (Auto) on 21st of Every Month

*/

/* User Table */


CREATE TABLE users (user_id varchar(10) PRIMARY KEY , username TEXT NOT NULL , password TEXT NOT NULL ,name TEXT NOT NULL ,  email TEXT NOT NULL , 
status varchar(15) NOT NULL DEFAULT 'active')ENGINE=INNODB;

CREATE TABLE country ( country_id varchar(10) PRIMARY KEY , country_name TEXT NOT NULL , country_rate DOUBLE NOT NULL DEFAULT 0.20)ENGINE=INNODB;

CREATE TABLE url ( url_id varchar(10) PRIMARY KEY , country_id varchar(10) , url_name TEXT NOT NULL , url TEXT NOT NULL , 
FOREIGN KEY (country_id) REFERENCES country(country_id))ENGINE=INNODB;

CREATE TABLE leads (lead_id varchar(10) PRIMARY KEY , 
generation_date TIMESTAMP , 
user_id varchar(10) ,
username TEXT NOT NULL ,  country_id varchar(10) , 
country TEXT NOT NULL , url_id varchar(10) , url TEXT NOT NULL ,  count BIGINT NOT NULL , 
FOREIGN KEY (user_id) REFERENCES users(user_id) , 
FOREIGN KEY (country_id) REFERENCES country(country_id) , 
FOREIGN KEY (url_id) REFERENCES url(url_id));


CREATE TABLE allowed_countries (allowed_id varchar(10) PRIMARY KEY , user_id varchar(10) , country_id varchar(10) ,
 FOREIGN KEY (user_id) REFERENCES users(user_id) , 
 FOREIGN KEY (country_id) REFERENCES country(country_id))ENGINE=INNODB;

insert into country values ('con_001' , 'SWEDEN' , '0.10');

insert into url values('url_001' , 'con_001' , 'hitta' , 'http://www.hitta.se/');
insert into url values('url_002' , 'con_001' , '118100' , 'http://www.118100.se/');

insert into allowed_countries values ('al_001' , 'user_001' , 'con_001');

CREATE TABLE balance ( balance_id varchar(10) PRIMARY KEY , user_id varchar(10) , country_id varchar(10) , balance DOUBLE ,
FOREIGN KEY (user_id) REFERENCES users(user_id) , 
 FOREIGN KEY (country_id) REFERENCES country(country_id))ENGINE=INNODB;


insert into balance values('bal_001' , 'user_001' , 'con_001' , 100000);

