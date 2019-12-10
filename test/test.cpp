
#define DEBUG

#include <iostream>
#include <string>
#include "qtest.hpp"
#include "dbfs.hpp"

using namespace std;

DESCRIBE("DBFS", {
	
	DBFS::root = "tmp";
	
	IT("should create file called `abcdef` in ./tmp/ab/cd dir", {
		DBFS::create("abcdef")->close();
		EXPECT(DBFS::exists("abcdef")).toBe(true);
		DBFS::remove("abcdef");
	});
	
	DESCRIBE("Random name file is created", {
		
		DBFS::File* f;
		BEFORE_ALL({
			f = DBFS::create();
		});
		
		AFTER_ALL({
			delete f;
		});
		
		IT("File length should be equal to " + to_string(DBFS::filelength), {
			EXPECT(f->name().size()).toBe(DBFS::filelength);
		});
		
		IT("File name should contain only alphabetic letters and numbers", {
			string name = f->name();
			for(int i=0;i<name.size();i++){
				char c = name[i];
				if( (c < '0' || c > '9') && (c < 'a' || c > 'z') )
					TEST_FAILED();
			}
			TEST_SUCCEED();
		});
		
		IT("File should exists", {
			DBFS::exists(f->name());
		});
		
		DESCRIBE("Move file to new location", {
			string oldname = f->name();
			string newname = DBFS::random_filename();
			
			BEFORE_ALL({				
				f->move(newname);
			});
			
			IT("File old location should not exists", {
				EXPECT(DBFS::exists(oldname)).toBe(false);
			});
			
			IT("File new location should exists", {
				EXPECT(DBFS::exists(newname)).toBe(true);
			});
		});
		
		DESCRIBE("Remove file", {
			
			string name;
			
			BEFORE_ALL({
				name = f->name();
				f->remove();
			});
			
			IT("file should be closed", {
				EXPECT(f->is_open()).toBe(false);
			});
			
			IT("File location should not exists", {
				EXPECT(DBFS::exists(name)).toBe(false);
			});
		});
	});
	
	DESCRIBE("File Created", {
		DBFS::File* f;
		
		BEFORE_ALL({
			f = DBFS::create();
		});
		
		AFTER_ALL({
			f->remove();
			delete f;
		});
		
		DESCRIBE("Write 10 numbers to file", {
			BEFORE_ALL({
				for(int i=0;i<10;i++){
					f->write(i);
				}
				f->write('\n');
			});
			
			IT("file size should be equal 11", {
				EXPECT(f->size()).toBe(11);
				INFO_PRINT("File size: " + to_string(f->size()));
			});
			
			DESCRIBE("Seek for 4th bit", {
				BEFORE_ALL({
					f->seek(4);
				});
				
				IT("current char should be equal `4`", {
					char c;
					f->read(c);
					
					EXPECT(c).toBe('4');
				});
			});
			
			DESCRIBE("Seek for 2th bit and put 3 numbers", {
				
				BEFORE_ALL({				
					f->seek(1);
					for(int i=1;i<=3;i++){
						f->write(i);
					}
				});
				
				IT("tell should be equal to `4`", {
					EXPECT(f->tell()).toBe(4);
				});
				
				IT("next char should be equal `4", {
					char c;
					f->read(c);
					EXPECT(c).toBe('4');
				});
			});
			
			DESCRIBE("Seek for 6th bit and read int", {
				int a;
				BEFORE_ALL({
					f->seek(6);
					f->read(a);
				});
				
				IT("`a` should be equal to `6789`", {
					EXPECT(a).toBe(6789);
				});
			});
			
			DESCRIBE("Seek for 0th bit and read char buffer of size()", {
				char* buf = new char[50];
				BEFORE_ALL({
					f->seek(0);
					int sz = f->size();
					f->read(buf, --sz);
					buf[sz] = '\0';
				});
				
				IT("buf should be equal to 0123456789", {
					EXPECT(string(buf)).toBe("0123456789");
					INFO_PRINT(string(buf));
				});
				
				IT("File should not be fail", {
					EXPECT(f->fail()).toBe(false);
				});
			});
		});
	});
});


int main(){
	
	return 0;
}
