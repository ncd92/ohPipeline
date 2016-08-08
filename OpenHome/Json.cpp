#include <OpenHome/Json.h>
#include <OpenHome/Types.h>
#include <OpenHome/Buffer.h>
#include <OpenHome/Private/Ascii.h>
#include <OpenHome/Private/Printer.h>
#include <OpenHome/Private/Stream.h>

#include <map>

using namespace OpenHome;

// Json

// see RFC4627 - http://www.ietf.org/rfc/rfc4627.txt

const Brn Json::kEscapedDoubleQuote("\\\"");
const Brn Json::kEscapedBackslash("\\\\");
const Brn Json::kEscapedForwardSlash("\\/");
const Brn Json::kEscapedBackspace("\\b");
const Brn Json::kEscapedFormfeed("\\f");
const Brn Json::kEscapedNewline("\\n");
const Brn Json::kEscapedLinefeed("\\r");
const Brn Json::kEscapedTab("\\t");

void Json::Escape(IWriter& aWriter, const Brx& aValue)
{
    // FIXME - no support for multi-byte chars
    const TUint bytes = aValue.Bytes();
    for (TUint i=0; i<bytes; i++) {
        TByte ch = aValue[i];
        switch (ch)
        {
        case '\"':
            aWriter.Write(kEscapedDoubleQuote);
            break;
        case '\\':
            aWriter.Write(kEscapedBackslash);
            break;
        case '/':
            aWriter.Write(kEscapedForwardSlash);
            break;
        case '\b':
            aWriter.Write(kEscapedBackspace);
            break;
        case '\f':
            aWriter.Write(kEscapedFormfeed);
            break;
        case '\n':
            aWriter.Write(kEscapedNewline);
            break;
        case '\r':
            aWriter.Write(kEscapedLinefeed);
            break;
        case '\t':
            aWriter.Write(kEscapedTab);
            break;
        default:
            if (ch > 0x1F) {
                aWriter.Write(ch);
            }
            else {
                Bws<6> hexBuf("\\u00");
                Ascii::AppendHex(hexBuf, ch);
                aWriter.Write(hexBuf);
            }
            break;
        }
    }
}

void Json::Unescape(Bwx& aValue)
{
    TUint j = 0;
    const TUint bytes = aValue.Bytes();
    for (TUint i=0; i<bytes; i++) {
        TByte ch = aValue[i];
        if (ch != '\\') {
            aValue[j++] = ch;
        }
        else {
            if (++i == bytes) {
                THROW(JsonInvalid);
            }
            switch (aValue[i])
            {
            case '\"':
                aValue[j++] = '\"';
                break;
            case '\\':
                aValue[j++] = '\\';
                break;
            case '/':
                aValue[j++] = '/';
                break;
            case 'b':
                aValue[j++] = '\b';
                break;
            case 'f':
                aValue[j++] = '\f';
                break;
            case 'n':
                aValue[j++] = '\n';
                break;
            case 'r':
                aValue[j++] = '\r';
                break;
            case 't':
                aValue[j++] = '\t';
                break;
            case 'u':
            {
                if (i+4 >= bytes) {
                    THROW(JsonInvalid);
                }
                Brn hexBuf = aValue.Split(i+1, 4);
                i += 4;
                const TUint hex = Ascii::UintHex(hexBuf);
                if (hex > 0xFF) {
                    THROW(JsonUnsupported);
                }
                aValue[j++] = (TByte)hex;
            }
                break;
            default:
                THROW(JsonInvalid);
            }
        }
    }
    aValue.SetBytes(j);
}


// JsonParser

JsonParser::JsonParser()
{
}

void JsonParser::Reset()
{
    iPairs.clear();
}

inline void JsonParser::Add(const Brn& aKey, const TByte* aValStart, TUint aValBytes)
{
    Brn val(aValStart, aValBytes);
//    Log::Print("Add %.*s, %.*s\n", PBUF(aKey), PBUF(val));
    iPairs.insert(std::pair<Brn, Brn>(aKey, val));
}

void JsonParser::Parse(const Brx& aJson, TBool aUnescapeInPlace)
{
    Reset();

    const TByte* ptr = aJson.Ptr();
    const TByte* end = ptr + aJson.Bytes();

    enum ParseState {
        DocStart,
        KeyStart,
        KeyEnd,
        ValueStart,
        NumEnd,
        StringEnd,
        ArrayEnd,
        ObjectEnd,
        MiscEnd,
        Complete
    } state;
    state = DocStart;
    const TByte* keyStart = nullptr;
    const TByte* valStart = nullptr;
    Brn key;
    TUint nestCount = 0;

    while (state != Complete && ptr < end) {
        TChar ch = (TChar)*ptr++;
        if (Ascii::IsWhitespace(ch)) {
            continue;
        }
        switch (state)
        {
        case DocStart:
            if (ch == '{') {
                state = KeyStart;
            }
            break;
        case KeyStart:
            switch (ch)
            {
            case '\"':
                keyStart = ptr;
                state = KeyEnd;
                break;
            case '}':
                state = Complete;
                break;
            default:
                if (ch != ',') {
                    THROW(JsonCorrupt);
                }
                break;
            }
            break;
        case KeyEnd:
            if (ch == '\"') {
                key.Set(keyStart, ptr - keyStart - 1);
                state = ValueStart;
            }
            break;
        case ValueStart:
            if (ch != ':') {
                if (ch == '\"') {
                    valStart = ptr;
                    state = StringEnd;
                }
                else {
                    valStart = ptr - 1;
                    if (ch == '[') {
                        state = ArrayEnd;
                        nestCount = 1;
                    }
                    else if (ch == '{') {
                        state = ObjectEnd;
                        nestCount = 1;
                    }
                    else if (ch == '-' || Ascii::IsDigit(ch)) {
                        // FIXME - no support for frac or exp
                        state = NumEnd;
                    }
                    else {
                        state = MiscEnd;
                    }
                }
            }
            break;
        case NumEnd:
        case MiscEnd:
            if (ch == ',') {
                Add(key, valStart, ptr - valStart - 1);
                state = KeyStart;
            }
            break;
        case StringEnd:
            if (ch == '\"') {
                const TUint bytes = ptr - valStart - 1;
                if (!aUnescapeInPlace) {
                    Add(key, valStart, bytes);
                }
                else {
                    Bwn buf(valStart, bytes, bytes);
                    Json::Unescape(buf);
                    Add(key, buf.Ptr(), buf.Bytes());
                }
                state = KeyStart;
            }
            break;
        case ArrayEnd:
            if (ch == '[') {
                nestCount++;
            }
            else if (ch == ']') {
                nestCount--;
                if (nestCount == 0) {
                    Add(key, valStart, ptr - valStart);
                    state = KeyStart;
                }
            }
            break;
        case ObjectEnd:
            if (ch == '{') {
                nestCount++;
            }
            else if (ch == '}') {
                nestCount--;
                if (nestCount == 0) {
                    Add(key, valStart, ptr - valStart);
                    state = KeyStart;
                }
            }
            break;
        default:
            ASSERTS();
        }

    }

    if (state != Complete) {
        THROW(JsonCorrupt);
    }
}

TBool JsonParser::HasKey(const TChar* aKey) const
{
    Brn key(aKey);
    return HasKey(key);
}

TBool JsonParser::HasKey(const Brx& aKey) const
{
    try {
        (void)Value(aKey);
        return true;
    }
    catch (JsonKeyNotFound&) {
        return false;
    }
}

Brn JsonParser::String(const TChar* aKey) const
{
    Brn key(aKey);
    return String(key);
}

Brn JsonParser::String(const Brx& aKey) const
{
    return Value(aKey);
}

TInt JsonParser::Num(const TChar* aKey) const
{
    Brn key(aKey);
    return Num(key);
}

TInt JsonParser::Num(const Brx& aKey) const
{
    try {
        Brn numBuf = Value(aKey);
        return Ascii::Int(numBuf);
    }
    catch (AsciiError&) {
        THROW(JsonCorrupt);
    }
}

Brn JsonParser::Value(const Brx& aKey) const
{
    Brn key(aKey);
    const auto it = iPairs.find(key);
    if (it == iPairs.end()) {
        THROW(JsonKeyNotFound);
    }
    return it->second;
}


// class WriterJson

const Brn WriterJson::kQuote("\"");
const Brn WriterJson::kSeparator(",");
const Brn WriterJson::kBoolTrue("true");
const Brn WriterJson::kBoolFalse("false");
const Brn WriterJson::kNull("null");

void WriterJson::WriteValueInt(IWriter& aWriter, TInt aValue)
{ // static
    Bws<Ascii::kMaxIntStringBytes> valBuf;
    (void)Ascii::AppendDec(valBuf, aValue);
    aWriter.Write(valBuf);
}

void WriterJson::WriteValueString(IWriter& aWriter, const Brx& aValue)
{ // static
    aWriter.Write(kQuote);
    Json::Escape(aWriter, aValue);
    aWriter.Write(kQuote);
}

void WriterJson::WriteValueBool(IWriter& aWriter, TBool aValue)
{ // static
    aWriter.Write(aValue? kBoolTrue : kBoolFalse);
}


// WriterJsonObject

const Brn WriterJsonObject::kObjectStart("{");
const Brn WriterJsonObject::kObjectEnd("}");

void WriterJsonObject::WriteKey(const TChar* aKey)
{
    WriteKey(Brn(aKey));
}

void WriterJsonObject::WriteKey(const Brx& aKey)
{
    if (iWrittenFirstKey) {
        iWriter->Write(WriterJson::kSeparator);
    }
    iWriter->Write(WriterJson::kQuote);
    Brn key(aKey);
    iWriter->Write(key);
    iWriter->Write(WriterJson::kQuote);
    iWriter->Write(Brn(":"));
    iWrittenFirstKey = true;
}

void WriterJsonObject::WriteInt(const TChar* aKey, TInt aValue)
{
    WriteInt(Brn(aKey), aValue);
}

void WriterJsonObject::WriteInt(const Brx& aKey, TInt aValue)
{
    CheckStarted();
    WriteKey(aKey);
    WriterJson::WriteValueInt(*iWriter, aValue);
}

void WriterJsonObject::WriteString(const TChar* aKey, const TChar* aValue)
{
    WriteString(Brn(aKey), Brn(aValue));
}

void WriterJsonObject::WriteString(const TChar* aKey, const Brx& aValue)
{
    WriteString(Brn(aKey), aValue);
}

void WriterJsonObject::WriteString(const Brx& aKey, const Brx& aValue)
{
    CheckStarted();
    WriteKey(aKey);
    WriterJson::WriteValueString(*iWriter, aValue);
}

void WriterJsonObject::WriteBool(const TChar* aKey, TBool aValue)
{
    WriteBool(Brn(aKey), aValue);
}

void WriterJsonObject::WriteBool(const Brx& aKey, TBool aValue)
{
    CheckStarted();
    WriteKey(aKey);
    WriterJson::WriteValueBool(*iWriter, aValue);
}

WriterJsonArray WriterJsonObject::CreateArray(const TChar* aKey)
{
    return CreateArray(Brn(aKey));
}

WriterJsonArray WriterJsonObject::CreateArray(const Brx& aKey)
{
    CheckStarted();
    WriteKey(aKey);
    WriterJsonArray writer(*iWriter);
    return writer;
}

WriterJsonObject WriterJsonObject::CreateObject(const TChar* aKey)
{
    return CreateObject(Brn(aKey));
}

WriterJsonObject WriterJsonObject::CreateObject(const Brx& aKey)
{
    CheckStarted();
    WriteKey(aKey);
    WriterJsonObject writer(*iWriter);
    return writer;
}

void WriterJsonObject::WriteEnd()
{
    if (iStarted) {
        iWriter->Write(kObjectEnd);
    }
    else {
        iWriter->Write(WriterJson::kNull);
    }
    iEnded = true;
}

WriterJsonObject::WriterJsonObject(IWriter& aWriter)
    : iWriter(&aWriter)
    , iStarted(false)
    , iEnded(false)
    , iWrittenFirstKey(false)
{
}

void WriterJsonObject::CheckStarted()
{
    ASSERT(!iEnded);
    if (!iStarted) {
        iWriter->Write(kObjectStart);
        iStarted = true;
    }
}


// WriterJsonDocument

WriterJsonDocument::WriterJsonDocument(IWriter& aWriter)
    : WriterJsonObject(aWriter)
{
}

void WriterJsonDocument::WriteEnd()
{
    CheckStarted();
    iWriter->Write(kObjectEnd);
    iEnded = true;
}


// WriterJsonArray

const Brn WriterJsonArray::kArrayStart("[");
const Brn WriterJsonArray::kArrayEnd("]");

WriterJsonArray::WriterJsonArray(IWriter& aWriter)
    : iWriter(&aWriter)
    , iStarted(false)
    , iEnded(false)
{
}

void WriterJsonArray::WriteInt(TInt aValue)
{
    WriteStartOrSeparator();
    WriterJson::WriteValueInt(*iWriter, aValue);
}

void WriterJsonArray::WriteString(const Brx& aValue)
{
    WriteStartOrSeparator();
    WriterJson::WriteValueString(*iWriter, aValue);
}

void WriterJsonArray::WriteBool(TBool aValue)
{
    WriteStartOrSeparator();
    WriterJson::WriteValueBool(*iWriter, aValue);
}

WriterJsonArray WriterJsonArray::CreateArray()
{
    WriteStartOrSeparator();
    WriterJsonArray writer(*iWriter);
    return writer;
}

WriterJsonObject WriterJsonArray::CreateObject()
{
    WriteStartOrSeparator();
    WriterJsonObject writer(*iWriter);
    return writer;
}

void WriterJsonArray::WriteEnd()
{
    if (iStarted) {
        iWriter->Write(kArrayEnd);
    }
    else {
        iWriter->Write(WriterJson::kNull);
    }
    iEnded = true;
}

void WriterJsonArray::WriteStartOrSeparator()
{
    ASSERT(!iEnded);
    if (iStarted) {
        iWriter->Write(WriterJson::kSeparator);
    }
    else {
        iWriter->Write(kArrayStart);
        iStarted = true;
    }
}
