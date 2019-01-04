////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#ifndef _LZWREAD_H_
#ifndef _LZWREAD_H_
#define _LZWREAD_H_

#include "lzw.h"


/*****************************************************************************
	LZWIO - abstraction of the input and output buffers, used to allow
	the caller to do buffer management in whatever fashion is appropriate.
******************************************************************* JohnBo **/
struct LZWIO
	{
	inline void LZWIOInit()
    {
		m_pbIn        = NULL;
        m_cbIn        = 0;
        m_pbOut       = NULL;
        m_cbOut       = 0;
		m_fNeedInput  = true;
        m_fNeedOutput = true;
    }

	const unsigned char *m_pbIn;
	int              m_cbIn;          // Description of the input buffer
	unsigned char *m_pbOut;
	int              m_cbOut;         // Description of the output buffer
	bool             m_fNeedInput;    // Set when input required
	bool             m_fNeedOutput;   // Set when output space required
	};


/*****************************************************************************
	The decompressor class - an LZWState and an LZWIO, the latter being
	public.
******************************************************************* JohnBo **/
struct LZWDecompressor : private LZWState, public LZWIO
	{
public:
	inline void LZWDecompressorInit(unsigned char bcodeSize)
    {
		m_chPrev = 0;
		m_fEnded = false;
		m_fError = false;
		m_iInput = 0;
		m_cbInput = 0;
		m_iTokenPrev = WClear();
		LZWStateInit(bcodeSize);
		LZWIOInit();

		LZW_ASSERT(bcodeSize < 12);

		/* The initial elements of the token array must be preset to single
			character tokens.  Note that, in the monochrome case, there will
			be two invalid tokens in there! */
		for (int i=0; i<WClear(); ++i)
			m_rgtoken[i] = ChToken((unsigned char)i);

		/* Executing Clear() at this point resets the rest of the token array,
			it does also reset the clear code and the EOD code. */
		Clear();
	}

	/* Process the data stream.  The API returns false at the end of the
		stream or after a terminal error, otherwise it returns true. */
	inline bool FProcess(void)
		{
		if (m_fEnded || m_fError) return false;
		LZW_ASSERT(!m_fNeedInput && !m_fNeedOutput);
		LZW_ASSERT(m_pbIn != NULL && m_pbOut != NULL);
		LZW_ASSERT(m_cbIn >= 0 && m_cbOut >= 0);
		return FHandleNext();
		}

	/* Has an error been encountered? */
	inline bool FLZWError(void) const { return m_fError; }
	inline bool FLZWEOF(void) const { return m_fEnded; }

	/* Reset the decompressor state.  This API is provided so the same object
		can be used to decompress multiple images within a GIF stream.  The
		LZWIO object is not reset, the caller must ensure that it is set up
		correctly. */
	void Reset(unsigned char bcodeSize);

protected:

private:
	/* Constants. */
	enum
		{
		lengthShift = (32-12),     // Where a token length is stored in the value
		};

	/* PRIVATE DATA */
	unsigned char   m_chPrev;     // The first character of the previous token
	bool            m_fEnded;     // Set after the zero length block has been seen
	bool            m_fError;     // A terminal error has been encountered

	/* The input buffer. */
	int             m_iInput;     // Input bits
	int             m_cbInput;    // Count of pending input bits
	int             m_iTokenPrev; // The previous output token

	/* The token table. */
	TokenValue m_rgtoken[ctokens];

	/* PRIVATE METHODS */
	/* Handle any available data, returning false only on EOD or terminal error. */
	bool FHandleNext(void);

	/* Clear the state of the decoder. */
	inline void Clear(void)
		{
		/* Note the wackiness to handle the fact that the predefined codes fill
			up two entries in the table - so we start with 2bpp for a 1bpp image,
			(i.e. the first code will really take 3 bits.) */
		m_ibitsToken = LZW_B8(m_bcodeSize+1);
		/* The "next token" must allow for the GIF clear code and the EOD
			code, so the "last" token is set to WEOD(). */
		m_itokenLast = WEOD();
		/* Only the generated tokens can be cleared - the others must be left
			untouched. */
		memset(m_rgtoken + WClear(), 0,
			(ctokens - WClear()) * sizeof m_rgtoken[0]);
		}

	/* Accessors on TokenValues.  These define the format of a token value, it is
		simply:

			length(12bits) previous(12bits) last character(8 bits)

		with the length in the most significant bits. */
	static inline int ILen(TokenValue tokenValue)
		{
		return tokenValue >> lengthShift;
		}

	static inline int IPrev(TokenValue tokenValue)
		{
		return (tokenValue >> 8) & (ctokens-1);
		}

	static unsigned char Ch(TokenValue tokenValue)
		{
		return (unsigned char)tokenValue;
		}

	/* Constructors for token values.  These construct a token directly from a
		character (it will be a terminal token) or from a previous token. */
	static inline TokenValue ChToken(unsigned char ch)
		{
		return ch + (1U<<lengthShift);
		}

	static inline TokenValue NextToken(int iTokenPrev, TokenValue tokenPrev,
		unsigned char ch)
		{
		LZW_ASSERT(iTokenPrev < ctokens);
		return ((tokenPrev + (1U<<lengthShift)) & (0xFFFU << lengthShift)) +
			(iTokenPrev << 8) + ch;
		}
	};

#endif
