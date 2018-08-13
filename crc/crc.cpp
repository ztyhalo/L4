const unsigned short cnCRC_16 = 0x8005;
unsigned long Table_CRC[256]; // CRC 表
// 构造 16 位 CRC 表
void BuildTable16( unsigned short aPoly )
{
	unsigned short i, j;
	unsigned short nData;
	unsigned short nAccum;
	for ( i = 0; i < 256; i++ )
	{
		nData = ( unsigned short )( i << 8 );
		nAccum = 0;
		for ( j = 0; j < 8; j++ )
		{
			if ( ( nData ^ nAccum ) & 0x8000 )
			nAccum = ( nAccum << 1 ) ^ aPoly;
			else
			nAccum <<= 1;
			nData <<= 1;
		}
		Table_CRC[i] = ( unsigned long )nAccum;
	}
}


unsigned short CRC_16( unsigned char * aData, unsigned long aSize )
{
	unsigned long i;
	unsigned short nAccum = 0;
	BuildTable16( cnCRC_16 ); // or cnCRC_CC99vT
	for ( i = 0; i < aSize; i++ )
	nAccum = ( nAccum << 8 ) ^ ( unsigned short )Table_CRC[( nAccum >> 8 ) ^ *aData++];
	return nAccum;
}

