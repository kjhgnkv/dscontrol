#pragma once
#include <Poco/RefCountedObject.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/AutoPtr.h>

#define MAX_BUF 4096
#define BLOCK_SIZE 1024

#define COMMAND_OFFSET 2
#define LENGTH_OFFSET 3
#define DATA_OFFSET 5
#define MULTI_INDEX1 5
#define MULTI_INDEX2 6
#define INDEX_ERROR 0xff
#define FILE_CONFIRMATION_OFFSET 7
#define MULTI_COMPLETE_OFFSET 7
#define COMMAND_RESULT 7
#define MULTI_DATA_OFFSET 8

#define PACKAGE_BEGIN 0x55
#define PACKAGE_END 0xaa

#define CMD_HEARTBEAT 0x01
#define CMD_RE_HEARTBEAT 0x02

#define CMD_DEPLOY 0x03
#define CMD_RE_DEPLOY 0x04

#define CMD_START 0x05
#define CMD_RE_START 0x06

#define CMD_UPDATE 0x07
#define CMD_RE_UPDATE 0x08

#define CMD_STOP 0x09
#define CMD_RE_STOP 0x0A

#define CMD_LEAVE 0x0B
#define CMD_RE_LEAVE 0x0C

#define CMD_FILE 0x0D
#define CMD_RE_FILE 0x0E

#define CMD_CHECK_DEPLOY 0x11
#define CMD_RE_CHECK_DEPLOY 0x12

#define CONFIRMATION 0x01
#define MULTI_COMPLETE 0x01
#define MULTI_NOT_COMPLETE 0x00
#define ADDITIONAL_SIZE (DATA_OFFSET+2)
#define MULTI_ADDITIONAL_SIZE (MULTI_DATA_OFFSET+2)

#define COMMAND_WAITING 0x03
#define COMMAND_RUNNING 0x02
#define COMMAND_DONE 0x01
#define COMMAND_FAIL 0x00

#define MAX_HEARD_COUNT 5

using Poco::RefCountedObject;
using Poco::Net::DatagramSocket;
using Poco::Net::SocketAddress;
using Poco::RefCountedObject;
using Poco::AutoPtr;

class Command : public RefCountedObject
{
public:
	virtual bool isMultiComplete() = 0;
protected:
	unsigned char* _buffer;
	size_t _size;
	char _command;
};
class SendCommand : public Command
{
public:
	typedef AutoPtr<SendCommand> Ptr;
	explicit SendCommand(Poco::Net::DatagramSocket& socket, char cmd, bool multiPackage=false)
		:_isMultiPackage(multiPackage), _isError(false), _socket(socket){
		_command = cmd;
		_buffer = NULL;
		_size=0;
	}
	~SendCommand() {
		if (_buffer)
			delete[]_buffer;
	}
	
	void setData(unsigned char* buffer, size_t size, short index=0, bool finish=false )
	{
		if (_buffer)
			delete []_buffer;
		_size = size;
		_buffer = new unsigned char[size];

		memcpy(_buffer, buffer, size);
		_fileIndex=index;
		_fileComplete= finish;
	}

	void setError(short index, char errorCode) {
		_size = 0;
		_fileIndex = index;
		_isError=true;
		_errorCode = errorCode;
	}

	bool isMultiComplete() { return _buffer[MULTI_COMPLETE_OFFSET]; }
	bool sendCommand(Poco::Net::SocketAddress address) {
		unsigned char sendBuf[MAX_BUF];
		size_t sendSize = constructSendBuffer(sendBuf, MAX_BUF);
		int sendCount=_socket.sendTo(sendBuf, sendSize, address);
		return sendCount= sendSize;
	}
private:
	Poco::Net::DatagramSocket& _socket;
	bool _isMultiPackage;
	short _fileIndex;
	bool _fileComplete;
	bool _isError;
	short _errorCode;
	void addMultiFlag(unsigned char* buffer)
	{
		buffer[MULTI_INDEX1] = _fileIndex >> 8;
		buffer[MULTI_INDEX2] = _fileIndex & 0xff;
		buffer[MULTI_COMPLETE_OFFSET] = _fileComplete ? MULTI_COMPLETE : MULTI_NOT_COMPLETE;
	}
	size_t constructSendBuffer(unsigned char* buffer, size_t maxSize) {
		if (ADDITIONAL_SIZE + _size > maxSize)
			return 0;
		buffer[0] = PACKAGE_BEGIN;
		buffer[1] = PACKAGE_END;
		buffer[COMMAND_OFFSET] = _command;
		buffer[LENGTH_OFFSET] = (_size >> 8);
		buffer[LENGTH_OFFSET + 1] = _size & 0xff;
		if (_isMultiPackage)
		{
			addMultiFlag(buffer);
			if(_size>0)
				memcpy(buffer + MULTI_DATA_OFFSET, _buffer, _size);
			buffer[MULTI_DATA_OFFSET + _size] = PACKAGE_END;
			buffer[MULTI_DATA_OFFSET + _size + 1] = PACKAGE_BEGIN;
			return _size + MULTI_ADDITIONAL_SIZE;
		}
		else
		{
			if (_size > 0)
				memcpy(buffer + DATA_OFFSET, _buffer, _size);
			buffer[DATA_OFFSET + _size] = PACKAGE_END;
			buffer[DATA_OFFSET + _size + 1] = PACKAGE_BEGIN;
			return _size + ADDITIONAL_SIZE;
		}
	}
	void setCommandStatus(char status) {
		_buffer[MULTI_COMPLETE_OFFSET] = status;
	}
};
class RecieveCommand : public Command
{
public:
	typedef AutoPtr<RecieveCommand> Ptr;
	explicit  RecieveCommand(char* buf, size_t size, const SocketAddress& add) {
		_size = size;
		_buffer = new unsigned char[_size];
		_add = add;
		memcpy(_buffer, buf, size);
		_isValid = isValid();
	};
	~RecieveCommand()
	{
		delete[]_buffer;
	};

	bool isValid() {
		bool valid = _buffer[0] == PACKAGE_BEGIN;
		valid = valid && (_buffer[1] == PACKAGE_END);
		size_t length = getDataLenght();
		valid = valid && (DATA_OFFSET + length <= _size);
		valid = valid && (_buffer[DATA_OFFSET + length + 1] == PACKAGE_END);
		valid = valid && (_buffer[DATA_OFFSET + length + 2] == PACKAGE_BEGIN);
		return valid;
	};

	size_t getDataLenght() {
		size_t length = _buffer[LENGTH_OFFSET];
		length = length << 8;
		length += _buffer[LENGTH_OFFSET + 1];
		return length;
	};

	size_t getData(unsigned char* buf, size_t max)
	{
		if (max < _size - ADDITIONAL_SIZE)
			return 0;
		memcpy(buf, _buffer + DATA_OFFSET, _size - ADDITIONAL_SIZE);
		return _size;
	}
	size_t getMultiPakageContent(unsigned char* buf, size_t max)
	{
		short size = _buffer[LENGTH_OFFSET];
		size = size << 8;
		size+= _buffer[LENGTH_OFFSET+1];
		if (max < size - MULTI_ADDITIONAL_SIZE)
			return 0;
		memcpy(buf, _buffer + MULTI_DATA_OFFSET, size);
		return size;
	}
	char getCommand() {
		return _buffer[COMMAND_OFFSET];
	};
	SocketAddress getAddress() { return _add; };
	short getMultiIndex()
	{
		short index = _buffer[MULTI_INDEX1];
		index = index << 8;
		index += _buffer[MULTI_INDEX2];
		return index;
	}
	bool isMultiComplete() { 
		return _buffer[MULTI_COMPLETE_OFFSET]; 
	}
private:
	
	SocketAddress _add;
	bool _isValid;
};
