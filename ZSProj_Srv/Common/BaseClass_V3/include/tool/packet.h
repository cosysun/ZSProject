

#ifndef __PACKET_H__
#define __PACKET_H__

#include "../config.h"


GV_BEGIN

class BASECLASS_API CPacket_Buffer
{
public:
	//get buffer
	virtual Void* Get_Buffer(Void) = 0;
	//get buffer size
	virtual UInt Get_Buffer_Size(Void) = 0;
	//add reference
	virtual Void Add_Reference(Void) = 0;
	//sub reference
	virtual Void Del_Reference(Bool destroy = false) = 0;

protected:

private:

};

class BASECLASS_API CPacket
{
public:
	enum packet_byte_order
	{
		packet_byte_order_normal,
		packet_byte_order_net,
	};

public:
	//
	CPacket(Void)
	{
		Init(0, 0);
	}
	//
	CPacket(Void* buff, UInt buff_size, packet_byte_order byte_order = packet_byte_order_normal)
	{
		Init(buff, buff_size, byte_order);
	}
	//
	~CPacket(Void)
	{
		Destroy();
	}
	//
	Void Init(Void* buff, UInt buff_size, packet_byte_order byte_order = packet_byte_order_normal)
	{
		m_data_buff = REI_CAST(Char*, buff);
		m_buff_size = buff_size;
		m_data_size = 0;
		m_data_index = 0;
		m_byte_order = byte_order;
		m_packet_buffer = 0;
	}
	//
	Void Init(CPacket_Buffer* buffer_obj, packet_byte_order byte_order = packet_byte_order_normal)
	{
		m_data_buff = REI_CAST(Char*, buffer_obj->Get_Buffer());
		m_buff_size = buffer_obj->Get_Buffer_Size();
		m_data_size = 0;
		m_data_index = 0;
		m_byte_order = byte_order;
		m_packet_buffer = buffer_obj;
		m_packet_buffer->Add_Reference();
	}
	//
	Void Reset(Void)
	{
		m_data_size = 0;
		m_data_index = 0;
	}
	//set data size
	Void Set_Data_Size(UInt size) {m_data_size = size;}
	//get data size
	UInt Get_Data_Size(Void) {return m_data_size;}
	//get date
	Char* Get_Buffer(Void) {return m_data_buff;}
	//get buffer
	CPacket_Buffer* Get_Packet_Buffer(Void) {return m_packet_buffer;}
	//get current buffer
	Char* Get_Current_Buffer(Void) {return m_data_buff + m_data_index;}
	//set buffer
	Void Set_Buffer(Char* buff, UInt buff_size)
	{
		m_data_buff = buff;
		m_buff_size = buff_size;
	}
	//get buffer size
	UInt Get_Buff_Size(Void) {return m_buff_size;}
	//set data pos
	Void Set_Data_Index(Int index, Bool offset = false)
	{
		UInt result;
		if (offset)
			result = STA_CAST(Int, m_data_index) + index;
		else
			result = index;
		if (result <= m_buff_size)
		{
			m_data_index = result;
			if (m_data_index > m_data_size)
				m_data_size = m_data_index;
		}
	}
	//get data pos
	UInt Get_Data_Index(Void) {return m_data_index;}
	//
	Void Set_Byte_Order(packet_byte_order byte_order = packet_byte_order_normal) {m_byte_order = byte_order;}
	//release resource
	Void Destroy(Void)
	{
		if (m_packet_buffer)
			m_packet_buffer->Del_Reference();
	}
	//
protected:
	//
	Bool Is_Net_Byte_Order(Void) {return m_byte_order == packet_byte_order_net;}
	Char*        m_data_buff;//
	UInt         m_buff_size;//
	UInt         m_data_size;//
	UInt         m_data_index;//
	packet_byte_order m_byte_order;
	CPacket_Buffer* m_packet_buffer;//

private:

};

class BASECLASS_API CPacket_Read	: public CPacket
{
public:
	//
	Int8 Read_int8(Void)
	{
		Int8 ret_val = 0;
		UInt op_size = STA_CAST(UInt, sizeof(Int8));
		if (m_data_index + op_size <= m_data_size)
		{
			MEMORY_COPY(&ret_val, m_data_buff + m_data_index, op_size);
			m_data_index += op_size;
		}

		return ret_val;
	}
    //
	Int16 Read_int16(Void)
	{
		Int16 ret_val = 0;
		UInt op_size = STA_CAST(UInt, sizeof(Int16));
		if (m_data_index + op_size <= m_data_size)
		{
			MEMORY_COPY(&ret_val, m_data_buff + m_data_index, op_size);
			//
			m_data_index += op_size;
		}

		return ret_val;
	}
    //
	Int32 Read_int32(Void)
	{
		Int32 ret_val = 0;
		UInt op_size = STA_CAST(UInt, sizeof(Int32));
		if (m_data_index + op_size <= m_data_size)
		{
			MEMORY_COPY(&ret_val, m_data_buff + m_data_index, op_size);
			//
			m_data_index += op_size;
		}

		return ret_val;
	}
    //
	Void* Read_block(UInt len = 0)
	{
		Void* ret_val = 0;
		if (len == 0)
			len = m_data_size - m_data_index;
		if (len > 0 && m_data_index + len <= m_data_size)
		{
			ret_val = m_data_buff + m_data_index;
			m_data_index += len;
		}

		return ret_val;
	}
    //
	template <typename T>
	T* Read_block(Void)
	{
		T* ret_val = 0;
		UInt len = STA_CAST(UInt, sizeof(T));
		if (m_data_index + len <= m_data_size)
		{
			ret_val = REI_CAST(T*, m_data_buff + m_data_index);
			m_data_index += len;
		}

		return ret_val;
	}
	//
	Char* Read_string(Void)
	{
		Char* ret_val = 0;
		UInt data_index = m_data_index;
		while (m_data_buff[data_index] != 0)
		{
			data_index++;
			if (data_index >= m_data_size)
				return Null;
		}

		if (data_index < m_data_size)
		{
			ret_val = m_data_buff + m_data_index;
			m_data_index = data_index + 1;
		}

		return ret_val;
	}
	//
	UInt Get_Left_Len(Void) {return m_data_size - m_data_index;}

protected:

private:

};

class BASECLASS_API CPacket_Write : public CPacket
{
public:
	//
	Bool Write_int8(Int8 val)
	{
		UInt op_size = STA_CAST(UInt, sizeof(Int8));
		if (m_data_index + op_size <= m_buff_size)
		{
			MEMORY_COPY(m_data_buff + m_data_index, &val, op_size);
			m_data_index += op_size;
			if (m_data_index > m_data_size)
				m_data_size = m_data_index;
			return true;
		}
		else
			return false;
	}
	//
	Bool Write_int16(Int16 val)
	{
		UInt op_size = STA_CAST(UInt, sizeof(Int16));
		if (m_data_index + op_size <= m_buff_size)
		{
			//
			MEMORY_COPY(m_data_buff + m_data_index, &val, op_size);
			m_data_index += op_size;
			if (m_data_index > m_data_size)
				m_data_size = m_data_index;
			return true;
		}
		else
			return false;
	}
	//
	Bool Write_int32(Int32 val)
	{
		UInt op_size = STA_CAST(UInt, sizeof(Int32));
		if (m_data_index + op_size <= m_buff_size)
		{
			
			MEMORY_COPY(m_data_buff + m_data_index, &val, op_size);
			m_data_index += op_size;
			if (m_data_index > m_data_size)
				m_data_size = m_data_index;
			return true;
		}
		else
			return false;
	}
	//
	Bool Write_block(Void* val, UInt len)
	{
		if (m_data_index + len <= m_buff_size)
		{
			MEMORY_COPY(m_data_buff + m_data_index, val, len);
			m_data_index += len;
			if (m_data_index > m_data_size)
				m_data_size = m_data_index;
			return true;
		}
		else
			return false;
	}

	template <typename T>
	Bool Write_block(T* val)
	{
		UInt len = STA_CAST(UInt, sizeof(T));
		if (m_data_index + len <= m_buff_size)
		{
			MEMORY_COPY(m_data_buff + m_data_index, val, len);
			m_data_index += len;
			if (m_data_index > m_data_size)
				m_data_size = m_data_index;
			return true;
		}
		else
			return false;
	}
	//
	Bool Write_string(Char* val)
	{
		UInt len = STA_CAST(UInt, strlen(val)) + 1;
		return Write_block(val, len);
	}
	//
	Bool Write_fstring(Char* format, ...)
	{
		if (m_buff_size <= 0)
			return false;
		va_list args;
		va_start(args, format);
		Int len = vsnprintf(m_data_buff + m_data_index,m_buff_size-m_data_index, format, args);
		va_end(args);
		if (len == -1)
			return false;

		m_data_index += len;
		m_data_buff[m_data_index] = 0;
		m_data_index += 1;
		if (m_data_index > m_data_size)
			m_data_size = m_data_index;
		return true;
	}
	//
	UInt Get_Left_Len(Void) {return m_buff_size - m_data_index;}

protected:

private:

};

GV_END

#endif 
