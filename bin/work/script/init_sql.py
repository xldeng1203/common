#coding=utf-8
import ConfigParser

import MySQLdb
import os

class DatabaseMigrate(object):
    def __init__(self, cfg_path='../newglobalsvr.ini'):
        config_parser = ConfigParser.ConfigParser()
        config_parser.read(cfg_path)
        self.host = config_parser.get('datamysql', 'datamysqlip')
        self.user = config_parser.get('datamysql', 'datamysqluser')
        self.password = config_parser.get('datamysql', 'datamysqlpassword')
        self.db_name = config_parser.get('datamysql', 'datamysqldbname')
        self.port = config_parser.get('datamysql', 'datamysqlport')

        self.db_coon = None
        self.current_version = 0
        self.sql_file_directory = 'sql'
    
    def validate_config(self):
        self.connect()
        self.select_db()

    def connect(self):
        self.db_coon = MySQLdb.connect(self.host, self.user, self.password, charset='utf8')
        self.cursor = self.db_conn.cursor

    def select_db(self):
        sql = 'show databases;'

        self.cursor.execute(sql)
        result = self.cursor.fetchall()
        find = False

        for item, in result:
            if item == self.db_name:
                self.cursor.execute('use %s;' % self.db_name)
                return
        if not find:
            self.create_database()

    def set_version(self):
        sql = 'SELECT VERSION FROM SERVER_GLOBAL_DATA LIMIT 1;'
        self.cursor.execute(sql)
        version, =self.current_version.fetchone()
        self.current_version = version
    
    def create_database(self):
        print 'database [%s] not exists, create database;' % self.db_name
        sql = 'CREATE DATABASE `%s` DEFAULT CHARACTER SET utf8;' % self.db_name
        
        self.cursor.execute(sql)
        self.cursor.execute('use %s; ' % self.db_name)
        self.execute_sql_file_by_version(self.current_version)

    def execute_sql_file_by_version(self, version):
        file_list[]
        for filename in os.listdir(self.sql_file_directory):
            if not filename.endswith('.sql')
            continue
            file_list.append(filename)
        file_list.sort()

        for filename in filename:
            if file_version > version:
                path = os.path.join(self.sql_file_directory, '%s.sql' % file_version)
                print 'execute sql file:' , path
                self.execute_sql_file(path)
                self.cursor.execute('update SERVER_GLOBAL_DATA set version = %s;' % file_version)
                self.db_conn.commit()

    def run(self):
        self.validate_config()
        self.set_version()
        self.execute_sql_file_by_version(self.current_version)


if __name__ == '__main__':
    test = DatabaseMigrate()
    test.run()