package flex.messaging.io.amf;

import java.io.Externalizable;
import java.io.IOException;
import java.io.UTFDataFormatException;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import flex.messaging.io.AbstractProxy;
import flex.messaging.io.ClassAliasRegistry;
import flex.messaging.io.PropertyProxy;
import flex.messaging.io.PropertyProxyRegistry;
import flex.messaging.io.SerializationContext;
import flex.messaging.io.SerializationException;
import flex.messaging.io.UnknownTypeException;
import flex.messaging.io.amf.ASObject;
import flex.messaging.io.amf.AbstractAmfInput;
import flex.messaging.io.amf.Amf3Types;
import flex.messaging.io.amf.TraitsInfo;
import flex.messaging.util.ClassUtil;
import flex.messaging.util.Trace;

public class Amf3InputUpdated extends AbstractAmfInput implements Amf3Types {
	protected List objectTable;
	protected List stringTable;
	protected List traitsTable;
	private TraitsInfo tempTi;
	private boolean needTraitShapshot = false;

	public Amf3InputUpdated(SerializationContext context) {
		super(context);

		this.stringTable = new ArrayList(64);
		this.objectTable = new ArrayList(64);
		this.traitsTable = new ArrayList(10);
	}

	@Override
	public void reset() {
		super.reset();
		this.stringTable.clear();
		this.objectTable.clear();
		this.traitsTable.clear();
	}

	public Object saveObjectTable() {
		Object table = this.objectTable;
		this.objectTable = new ArrayList(64);
		return table;
	}

	public void restoreObjectTable(Object table) {
		this.objectTable = ((ArrayList) table);
	}

	public Object saveTraitsTable() {
		Object table = this.traitsTable;
		this.traitsTable = new ArrayList(10);
		return table;
	}

	public void restoreTraitsTable(Object table) {
		this.traitsTable = ((ArrayList) table);
	}

	public Object saveStringTable() {
		Object table = this.stringTable;
		this.stringTable = new ArrayList(64);
		return table;
	}

	public void restoreStringTable(Object table) {
		this.stringTable = ((ArrayList) table);
	}

	public Object readObject() throws ClassNotFoundException, IOException {
		int type = this.in.readByte();
		Object value = readObjectValue(type);
		return value;
	}

	protected Object readObjectValue(int type) throws ClassNotFoundException,
			IOException {
		Object value = null;

		switch (type) {
		case 6:
			value = readString();

			if (!(this.isDebug))
				break;
			this.trace.writeString((String) value);
			break;
		case 10:
			value = readScriptObject();
			break;
		case 9:
			value = readArray();
			break;
		case 2:
			value = Boolean.FALSE;

			if (!(this.isDebug))
				break;
			this.trace.write(value);
			break;
		case 3:
			value = Boolean.TRUE;

			if (!(this.isDebug))
				break;
			this.trace.write(value);
			break;
		case 4:
			int i = readUInt29();

			i = i << 3 >> 3;
			value = new Integer(i);

			if (!(this.isDebug))
				break;
			this.trace.write(value);
			break;
		case 5:
			value = new Double(this.in.readDouble());

			if (!(this.isDebug))
				break;
			this.trace.write(value);
			break;
		case 0:
			if (!(this.isDebug))
				break;
			this.trace.writeUndefined();
			break;
		case 1:
			if (!(this.isDebug))
				break;
			this.trace.writeNull();
			break;
		case 7:
		case 11:
			value = readXml();
			break;
		case 8:
			value = readDate();

			if (!(this.isDebug))
				break;
			this.trace.write(value.toString());
			break;
		case 12:
			value = readByteArray();
			break;
		case 13: // Vector.<int>
			value = readVectorInt();
			break;
		case 14: // Vector.<uint>
			value = readVectorUInt();
			break;
		case 15: // Vector.<Number>
			value = readVectorNumber();
			break;
		case 16: // Vector.<Object>
			value = readVectorObject();
			break;
		default:
			UnknownTypeException ex = new UnknownTypeException();
			ex.setMessage(10301, new Object[] { new Integer(type) });
			throw ex;
		}

		return value;
	}

	protected String readString() throws IOException {
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return getStringReference(ref >> 1);
		}

		int len = ref >> 1;

		if (0 == len) {
			return "";
		}

		String str = readUTF(len);

		this.stringTable.add(str);

		return str;
	}

	protected Date readDate() throws IOException {
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return ((Date) getObjectReference(ref >> 1));
		}

		long time = (long) this.in.readDouble();

		Date d = new Date(time);

		this.objectTable.add(d);

		if (this.isDebug) {
			this.trace.write(d);
		}
		return d;
	}

	protected Object readVectorInt() throws IOException {
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return getObjectReference(ref >> 1);
		}

		int len = ref >> 1;
		Object array = null;

		array = new ArrayList<Integer>(len);

		this.objectTable.add(array);
		int ref2 = readUInt29();
		for (int j = 0; j < len; ++j) {
			Integer obj = in.readInt();
			((List) array).add(obj);
		}

		return array;
	}

	protected Object readVectorUInt() throws IOException {
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return getObjectReference(ref >> 1);
		}

		int len = ref >> 1;
		Object array = null;

		array = new ArrayList<Long>(len);

		this.objectTable.add(array);
		int ref2 = readUInt29();
		for (int j = 0; j < len; ++j) {

			long valLong = (long) (in.readByte() & 0xFF) << 24L;
			valLong += (long) (in.readByte() & 0xFF) << 16L;
			valLong += (long) (in.readByte() & 0xFF) << 8L;
			valLong += (in.readByte() & 0xFF);
			Long obj = valLong;
			((List) array).add(obj);
		}

		return array;
	}

	protected Object readVectorNumber() throws IOException {
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return getObjectReference(ref >> 1);
		}

		int len = ref >> 1;
		Object array = null;

		array = new ArrayList<Double>(len);

		this.objectTable.add(array);
		int ref2 = readUInt29();
		for (int j = 0; j < len; ++j) {
			Double f = in.readDouble();
			((List) array).add(f);
		}

		return array;
	}

	protected Object readVectorObject() throws IOException 
	{
		//Start like all other objects
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return getObjectReference(ref >> 1);
		}

		int len = ref >> 1;
		Object array = null;

		//We can cast as the specific type. Class.forName("ArrayList<type>"); won't work :/
		array = new ArrayList<Object>(len);
		
		this.objectTable.add(array);
		
		//Vector of Object is like an pure object, it's got a traits defining it's type
		int ref2 = readUInt29();
		TraitsInfo ti = readTraits(ref2);
		String className = ti.getClassName();
		PropertyProxy proxy = null;
		
		//Optimization of the Vector traits re-write as been done in the AS3 serialization.
		//The traits is written once directly in the vector definition,
		//The class properties is written once, directly in the first object instance. but not in the folowing.
		
		needTraitShapshot = true;

		Object object;

		for (int j = 0; j < len; ++j) 
		{
			if ((className == null) || (className.length() == 0)) 
			{
				object = new ASObject();
			}
			else if (className.startsWith(">")) 
			{
				object = new ASObject();
				((ASObject) object).setType(className);
			}
			else if ((this.context.instantiateTypes) || (className.startsWith("flex."))) 
			{
				Class desiredClass = AbstractProxy.getClassFromClassName(className, this.context.createASObjectForMissingType);

				proxy = PropertyProxyRegistry.getRegistry().getProxyAndRegister(desiredClass);

				if (proxy == null) 
				{
					object = ClassUtil.createDefaultInstance(desiredClass, null);
				}
				else 
				{
					object = proxy.createInstance(className);
				}
			} 
			else 
			{
				object = new ASObject();
				((ASObject) object).setType(className);
			}

			if (proxy == null) 
			{
				proxy = PropertyProxyRegistry.getProxyAndRegister(object);
			}
			Object str = null;
			try 
			{
				str = readObject();
			}
			catch (ClassNotFoundException e) 
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			((List) array).add(str);
		}
		
		tempTi = null;
		needTraitShapshot = false;
		return array;
	}

	protected Object readArray() throws ClassNotFoundException, IOException {
		int i;
		Object item;
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return getObjectReference(ref >> 1);
		}

		int len = ref >> 1;
		Object array = null;

		Map map = null;
		while (true) {
			String name = readString();
			if (name == null)
				break;
			if (name.length() == 0)
				break;
			if (map == null) {
				map = new HashMap();
				array = map;

				this.objectTable.add(array);

				if (this.isDebug) {
					this.trace.startECMAArray(this.objectTable.size() - 1);
				}
			}
			Object value = readObject();
			map.put(name, value);
		}

		if (map == null) {
			if (this.context.legacyCollection) {
				List list = new ArrayList(len);
				array = list;

				this.objectTable.add(array);

				if (this.isDebug) {
					this.trace.startAMFArray(this.objectTable.size() - 1);
				}
				for (int j = 0; j < len; ++j) {
					if (this.isDebug) {
						this.trace.arrayElement(j);
					}
					Object obj = readObject();
					list.add(j, obj);
				}

			} else {
				array = new Object[len];

				this.objectTable.add(array);

				if (this.isDebug) {
					this.trace.startAMFArray(this.objectTable.size() - 1);
				}
				for (i = 0; i < len; ++i) {
					if (this.isDebug) {
						this.trace.arrayElement(i);
					}
					item = readObject();
					Array.set(array, i, item);
				}
			}

		} else {
			for (i = 0; i < len; ++i) {
				if (this.isDebug) {
					this.trace.arrayElement(i);
				}
				item = readObject();
				map.put(Integer.toString(i), item);
			}
		}

		if (this.isDebug) {
			this.trace.endAMFArray();
		}
		return array;
	}

	protected Object readScriptObject() throws ClassNotFoundException,
			IOException {
		Object object;
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return getObjectReference(ref >> 1);
		}


		TraitsInfo ti = readTraits(ref);
		if (needTraitShapshot)
		{
			needTraitShapshot = false;
			tempTi = ti;
		}
		
		if (tempTi != null)
		{
			ti = tempTi;
			//tempTi = null;
		}
		
		String className = ti.getClassName();
		
		boolean externalizable = ti.isExternalizable();

		PropertyProxy proxy = null;

		String aliasedClass = ClassAliasRegistry.getRegistry().getClassName(
				className);
		if (aliasedClass != null) {
			className = aliasedClass;
		}
		if ((className == null) || (className.length() == 0)) {
			object = new ASObject();
		} else if (className.startsWith(">")) {
			object = new ASObject();
			((ASObject) object).setType(className);
		} else if ((this.context.instantiateTypes)
				|| (className.startsWith("flex."))) {
			Class desiredClass = AbstractProxy.getClassFromClassName(className,
					this.context.createASObjectForMissingType);

			proxy = PropertyProxyRegistry.getRegistry().getProxyAndRegister(
					desiredClass);

			if (proxy == null)
				object = ClassUtil.createDefaultInstance(desiredClass, null);
			else {
				object = proxy.createInstance(className);
			}
		} else {
			object = new ASObject();
			((ASObject) object).setType(className);
		}

		if (proxy == null) {
			proxy = PropertyProxyRegistry.getProxyAndRegister(object);
		}

		int objectId = this.objectTable.size();
		this.objectTable.add(object);

		if (externalizable) {
			readExternalizable(className, object);
		} else {
			if (this.isDebug) {
				this.trace.startAMFObject(className,
						this.objectTable.size() - 1);
			}

			int len = ti.getProperties().size();

			for (int i = 0; i < len; ++i) {
				String propName = ti.getProperty(i);

				if (this.isDebug) {
					this.trace.namedElement(propName);
				}
				Object value = readObject();
				proxy.setValue(object, propName, value);
			}

			if (ti.isDynamic()) {
				while (true) {
					String name = readString();
					if (name == null)
						break;
					if (name.length() == 0)
						break;
					if (this.isDebug) {
						this.trace.namedElement(name);
					}
					Object value = readObject();
					proxy.setValue(object, name, value);
				}
			}
		}

		if (this.isDebug) {
			this.trace.endAMFObject();
		}

		Object newObj = proxy.instanceComplete(object);

		if (newObj != object) {
			this.objectTable.set(objectId, newObj);
			object = newObj;
		}

		return object;
	}

	protected void readExternalizable(String className, Object object)
			throws ClassNotFoundException, IOException {
		if (object instanceof Externalizable) {
			if (this.isDebug) {
				this.trace.startExternalizableObject(className,
						this.objectTable.size() - 1);
			}

			((Externalizable) object).readExternal(this);
		} else {
			SerializationException ex = new SerializationException();
			ex.setMessage(10305, new Object[] { object.getClass().getName() });
			throw ex;
		}
	}

	protected byte[] readByteArray() throws IOException {
		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			return ((byte[]) getObjectReference(ref >> 1));
		}

		int len = ref >> 1;

		byte[] ba = new byte[len];

		this.objectTable.add(ba);

		this.in.readFully(ba, 0, len);

		if (this.isDebug) {
			this.trace.startByteArray(this.objectTable.size() - 1, len);
		}
		return ba;
	}

	protected TraitsInfo readTraits(int ref) throws IOException {
		if ((ref & 0x3) == 1) {
			return getTraitReference(ref >> 2);
		}

		boolean externalizable = (ref & 0x4) == 4;
		boolean dynamic = (ref & 0x8) == 8;
		int count = ref >> 4;
		String className = readString();

		TraitsInfo ti = new TraitsInfo(className, dynamic, externalizable,
				count);

		this.traitsTable.add(ti);

		for (int i = 0; i < count; ++i) {
			String propName = readString();
			ti.addProperty(propName);
		}

		return ti;
	}

	protected String readUTF(int utflen) throws IOException {
		char[] charr = getTempCharArray(utflen);
		byte[] bytearr = getTempByteArray(utflen);

		int count = 0;
		int chCount = 0;

		this.in.readFully(bytearr, 0, utflen);

		while (count < utflen) {
			int char2;
			int c = bytearr[count] & 0xFF;
			switch (c >> 4) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				++count;
				charr[chCount] = (char) c;
				break;
			case 12:
			case 13:
				count += 2;
				if (count > utflen)
					throw new UTFDataFormatException();
				char2 = bytearr[(count - 1)];
				if ((char2 & 0xC0) != 128)
					throw new UTFDataFormatException();
				charr[chCount] = (char) ((c & 0x1F) << 6 | char2 & 0x3F);
				break;
			case 14:
				count += 3;
				if (count > utflen)
					throw new UTFDataFormatException();
				char2 = bytearr[(count - 2)];
				int char3 = bytearr[(count - 1)];
				if (((char2 & 0xC0) != 128) || ((char3 & 0xC0) != 128))
					throw new UTFDataFormatException();
				charr[chCount] = (char) ((c & 0xF) << 12 | (char2 & 0x3F) << 6 | (char3 & 0x3F) << 0);

				break;
			case 8:
			case 9:
			case 10:
			case 11:
			default:
				throw new UTFDataFormatException();
			}
			++chCount;
		}

		return new String(charr, 0, chCount);
	}

	protected int readUInt29() throws IOException {
		int b = this.in.readByte() & 0xFF;

		if (b < 128) {
			return b;
		}

		int value = (b & 0x7F) << 7;
		b = this.in.readByte() & 0xFF;

		if (b < 128) {
			return (value | b);
		}

		value = (value | b & 0x7F) << 7;
		b = this.in.readByte() & 0xFF;

		if (b < 128) {
			return (value | b);
		}

		value = (value | b & 0x7F) << 8;
		b = this.in.readByte() & 0xFF;

		return (value | b);
	}

	protected Object readXml() throws IOException {
		String xml = null;

		int ref = readUInt29();

		if ((ref & 0x1) == 0) {
			xml = (String) getObjectReference(ref >> 1);
		} else {
			int len = ref >> 1;

			if (0 == len)
				xml = "";
			else {
				xml = readUTF(len);
			}

			this.objectTable.add(xml);

			if (this.isDebug) {
				this.trace.write(xml);
			}
		}
		return stringToDocument(xml);
	}

	protected Object getObjectReference(int ref) {
		if (this.isDebug) {
			this.trace.writeRef(ref);
		}

		return this.objectTable.get(ref);
	}

	protected String getStringReference(int ref) {
		String str = (String) this.stringTable.get(ref);

		if ((Trace.amf) && (this.isDebug)) {
			this.trace.writeStringRef(ref);
		}

		return str;
	}

	protected TraitsInfo getTraitReference(int ref) {
		if ((Trace.amf) && (this.isDebug)) {
			this.trace.writeTraitsInfoRef(ref);
		}

		return ((TraitsInfo) this.traitsTable.get(ref));
	}
}
