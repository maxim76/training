"""
Example demostrates execution of an arbitrary sql statement with input and output parameters.
Class prepares statement before execution to make subsequent call be more efficient.
Transactions are supported for demonstration only (sql code used here does not modify any data).
"""

import cx_Oracle
import dbconfig


class DbRequest:
    '''
    Class executes a predefined statement in Oracle DB.
    Input parameter: arbitrary string
    Output parameters: current date/time, string that contains db version and first 4 bytes of hash, and full 16 bytes hash of input string
    '''

    SQL_STATEMENT='''
    declare
        l_hash4 NUMBER;
        l_pre10ihash NUMBER;
        l_version varchar(100);
        l_compatibility varchar2(100);
    begin
        l_hash4 := DBMS_UTILITY.GET_SQL_HASH(:i_string, :o_hash, l_pre10ihash);
        DBMS_UTILITY.DB_VERSION(l_version, l_compatibility);
        select sysdate into :o_date from dual;
        :o_string := CONCAT(l_version || ' ', l_hash4);
    end;
    '''
    
    def __init__(self):
        self.db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
        self.cursor = self.db.cursor()
        self.cursor.prepare(self.SQL_STATEMENT)

    def __del__(self):
        self.cursor.close()
        self.db.close()

    def execute(self, strParam):
        self.db.begin()
        try:
            o_date = self.cursor.var(cx_Oracle.DATETIME)
            o_hash = self.cursor.var(cx_Oracle.STRING)
            o_string = self.cursor.var(cx_Oracle.STRING)
            parameters = {
                'i_String'  : strParam,
                'o_date'    : o_date,
                'o_hash'    : o_hash,
                'o_string'  : o_string
            }
            self.cursor.execute(None, parameters)
            self.db.commit()
            return (o_date.getvalue(), o_hash.getvalue(), o_string.getvalue())
        except:
            self.db.rollback()


if __name__=="__main__":
    testCases = [
        'ABCD',
        'DEFG'
    ]

    db = DbRequest()
    for params in testCases:
        result = db.execute(params)
        print('Params: ' + str(params) + '   Result: ' + str(result))
