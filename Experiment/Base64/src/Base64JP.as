/* Base64 library for ActionScript 3.0. 
* 
* Copyright (C) 2007 Ma Bingyao <andot@ujn.edu.cn> 
* Version: 1.1 
* LastModified: Oct 26, 2007 
* This library is free.  You can redistribute it and/or modify it. 
*/   
/* 
* interfaces: 
* import org.phprpc.util.Base64; 
* import flash.utils.ByteArray; 
* var data:ByteArray = new ByteArray(); 
* data.writeUTFBytes("Hello PHPRPC"); 
* var b64:String = Base64.encode(data); 
* trace(b64); 
* trace(Base64.decode(b64)); 
*/   
package 
{   
	import com.frimastudio.Engine.Util.VectorUtil;
    import flash.utils.ByteArray;   
	import flash.utils.Endian;
	import Frima.Util.Console;
    public final class Base64JP {   
        private static  const encodeChars:Vector.<int> = InitEncoreChar();
        private static  const decodeChars:Array =    
        [-1, -1, -1, -1, -1, -1, -1, -1,   
        -1, -1, -1, -1, -1, -1, -1, -1,   
        -1, -1, -1, -1, -1, -1, -1, -1,   
        -1, -1, -1, -1, -1, -1, -1, -1,   
        -1, -1, -1, -1, -1, -1, -1, -1,   
        -1, -1, -1, 62, -1, -1, -1, 63,   
        52, 53, 54, 55, 56, 57, 58, 59,   
        60, 61, -1, -1, -1, -1, -1, -1,   
        -1,  0,  1,  2,  3,  4,  5,  6,   
         7,  8,  9, 10, 11, 12, 13, 14,   
        15, 16, 17, 18, 19, 20, 21, 22,   
        23, 24, 25, -1, -1, -1, -1, -1,   
        -1, 26, 27, 28, 29, 30, 31, 32,   
        33, 34, 35, 36, 37, 38, 39, 40,   
        41, 42, 43, 44, 45, 46, 47, 48,   
        49, 50, 51, -1, -1, -1, -1, -1];   
		
		public static function InitEncoreChar() : Vector.<int>
		{
			var encodeChars:Vector.<int> = new Vector.<int>();
			encodeChars.push(
			'A'.charCodeAt(), 'B'.charCodeAt(), 'C'.charCodeAt(),'D'.charCodeAt(), 'E'.charCodeAt(), 'F'.charCodeAt(), 'G'.charCodeAt(),
			'H'.charCodeAt(), 'I'.charCodeAt(), 'J'.charCodeAt(), 'K'.charCodeAt(),'L'.charCodeAt(), 'M'.charCodeAt(), 'N'.charCodeAt(), 'O'.charCodeAt(),
			'P'.charCodeAt(), 'Q'.charCodeAt(), 'R'.charCodeAt(), 'S'.charCodeAt(),'T'.charCodeAt(), 'U'.charCodeAt(), 'V'.charCodeAt(), 'W'.charCodeAt(),
			'X'.charCodeAt(), 'Y'.charCodeAt(),'Z'.charCodeAt(),'a'.charCodeAt(),'b'.charCodeAt(),'c'.charCodeAt(),'d'.charCodeAt(),'e'.charCodeAt(),'f'.charCodeAt(),   
			'g'.charCodeAt(),'h'.charCodeAt(),'i'.charCodeAt(),'j'.charCodeAt(),'k'.charCodeAt(),'l'.charCodeAt(),'m'.charCodeAt(),'n'.charCodeAt(),   
			'o'.charCodeAt(),'p'.charCodeAt(),'q'.charCodeAt(),'r'.charCodeAt(),'s'.charCodeAt(),'t'.charCodeAt(),'u'.charCodeAt(),'v'.charCodeAt(),   
			'w'.charCodeAt(),'x'.charCodeAt(),'y'.charCodeAt(),'z'.charCodeAt(),'0'.charCodeAt(),'1'.charCodeAt(),'2'.charCodeAt(),'3'.charCodeAt(),   
			'4'.charCodeAt(), '5'.charCodeAt(), '6'.charCodeAt(), '7'.charCodeAt(), '8'.charCodeAt(), '9'.charCodeAt(), '+'.charCodeAt(), '/'.charCodeAt());   
			
			return encodeChars;
		}
        public static function encode(data:ByteArray):String 
		{   
			var out:ByteArray = new ByteArray();   //Preset length
			out.length = (2 + data.length - ((data.length + 2) % 3)) * 4 / 3; //500: 1.6 to 1.5 ms
            //data.endian = Endian.BIG_ENDIAN;
			var i:int = 0;   
            var r:int = data.length % 3;   
            var len:int = data.length- r;   
            var c:int;   //read (3) character AND write (4) characters
            
			data.position = 0;
            while (i < len) 
			{   
				//Read 3 Characters (8bit * 3 = 24 bits)
                c = data[i++] << 16 | data[i++] << 8 | data[i++];   
				//Cannot optimize this to read int because of the positioning overhead.
				//Convert to 4 Characters (6 bit * 4 = 24 bits)
				c = (encodeChars[c >>> 18] << 24) | (encodeChars[c >>> 12 & 0x3f] << 16) | (encodeChars[c >>> 6 & 0x3f] << 8) | encodeChars[c & 0x3f];
				out.writeInt(c);
				// Optimization: Do one write Int instead of 4 write byte: 1.5 to 0.71 ms
            }   
            if (r == 1) //Need two "=" padding
			{   
				//Read one char, write two chars, write padding
                c = data[i];   
				c = (encodeChars[c >>> 2] << 24) | (encodeChars[(c & 0x03) << 4] << 16) | 61 << 8 | 61;
				out.writeInt(c);
            }   
            else if (r == 2) 
			{   
                c = data[i++] << 8 | data[i];   
				c = (encodeChars[c >>> 10] << 24) | (encodeChars[c >>> 4 & 0x3f] << 16) | (encodeChars[(c & 0x0f) << 2] << 8) | 61;
				out.writeInt(c);
            }   
            out.position = 0;
			//Console.Trace("out length "  + out.length);
			return out.readUTFBytes(out.length);   
        }   
        public static function decode(str:String):ByteArray {   
            var c1:int;   
            var c2:int;   
            var c3:int;   
            var c4:int;   
            var i:int;   
            var len:int;   
            var out:ByteArray;   
            len = str.length;   
            i = 0;   
            out = new ByteArray();   
            while (i < len) {   
                // c1   
                do {   
                    c1 = decodeChars[str.charCodeAt(i++) & 0xff];   
                } while (i < len && c1 == -1);   
                if (c1 == -1) {   
                    break;   
                }   
                // c2       
                do {   
                    c2 = decodeChars[str.charCodeAt(i++) & 0xff];   
                } while (i < len && c2 == -1);   
                if (c2 == -1) {   
                    break;   
                }   
                out.writeByte((c1 << 2) | ((c2 & 0x30) >> 4));   
                // c3   
                do {   
                    c3 = str.charCodeAt(i++) & 0xff;   
                    if (c3 == 61) {   
                        return out;   
                    }   
                    c3 = decodeChars[c3];   
                } while (i < len && c3 == -1);   
                if (c3 == -1) {   
                    break;   
                }   
                out.writeByte(((c2 & 0x0f) << 4) | ((c3 & 0x3c) >> 2));   
                // c4   
                do {   
                    c4 = str.charCodeAt(i++) & 0xff;   
                    if (c4 == 61) {   
                        return out;   
                    }   
                    c4 = decodeChars[c4];   
                } while (i < len && c4 == -1);   
                if (c4 == -1) {   
                    break;   
                }   
                out.writeByte(((c3 & 0x03) << 6) | c4);   
            }   
            return out;   
        }   
    }   
}