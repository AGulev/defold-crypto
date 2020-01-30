#include <dmsdk/sdk.h>
#include<malloc.h>

//#include <lua.h>
//#include <lauxlib.h>
#include "sha2.h"
#include "aes.h"

#define LIB_NAME "Crypto"
#define MODULE_NAME "crypto"

// source: md5lib.c from md5 Lua library
/**
*  X-Or. Does a bit-a-bit exclusive-or of two strings.
*  @param s1: arbitrary binary string.
*  @param s2: arbitrary binary string with same length as s1.
*  @return  a binary string with same length as s1 and s2,
*   where each bit is the exclusive-or of the corresponding bits in s1-s2.
*/
static int ex_or (lua_State *L) {
  size_t l1, l2;
  const char *s1 = luaL_checklstring(L, 1, &l1);
  const char *s2 = luaL_checklstring(L, 2, &l2);
  luaL_Buffer b;
  luaL_argcheck( L, l1 == l2, 2, "lengths must be equal" );
  luaL_buffinit(L, &b);
  while (l1--) luaL_putchar(&b, (*s1++)^(*s2++));
  luaL_pushresult(&b);
  return 1;
}

/**
*  @param: an arbitrary binary string.
*  @return: an SHA-256 hash string.
*/
static int sha256(lua_State *L) {
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	SHA256_CTX context;
	char digest[SHA256_DIGEST_LENGTH];

	SHA256_Init2(&context);
	SHA256_Update2(&context, (const sha2_byte *)data, len);
	SHA256_Final2((sha2_byte *)digest, &context);

	lua_pushlstring(L, digest, SHA256_DIGEST_LENGTH);
	return 1;
}

/**
*  @param: an arbitrary binary string.
*  @return: an SHA-256 hash string.
*/
static int sha256hex(lua_State *L) {
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	char digest[SHA256_DIGEST_STRING_LENGTH];
	SHA256_Data((const sha2_byte *)data, len, digest);
	lua_pushlstring(L, digest, SHA256_DIGEST_STRING_LENGTH - 1);
	return 1;
}

/**
*  @param: an arbitrary binary string.
*  @return: an SHA-384 hash string.
*/
static int sha384(lua_State *L) {
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	SHA384_CTX context;
	char digest[SHA384_DIGEST_LENGTH];

	SHA384_Init2(&context);
	SHA384_Update2(&context, (const sha2_byte *)data, len);
	SHA384_Final2((sha2_byte *)digest, &context);

	lua_pushlstring(L, digest, SHA384_DIGEST_LENGTH);
	return 1;
}

/**
*  @param: an arbitrary binary string.
*  @return: an SHA-384 hash string.
*/
static int sha384hex(lua_State *L) {
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	char digest[SHA384_DIGEST_STRING_LENGTH];
	SHA384_Data((const sha2_byte *)data, len, digest);
	lua_pushlstring(L, digest, SHA384_DIGEST_STRING_LENGTH - 1);
	return 1;
}

/**
*  @param: an arbitrary binary string.
*  @return: an SHA-512 hash string.
*/
static int sha512(lua_State *L) {
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	SHA512_CTX context;
	char digest[SHA512_DIGEST_LENGTH];

	SHA512_Init2(&context);
	SHA512_Update2(&context, (const sha2_byte *)data, len);
	SHA512_Final2((sha2_byte *)digest, &context);

	lua_pushlstring(L, digest, SHA512_DIGEST_LENGTH);
	return 1;
}

/**
*  @param: an arbitrary binary string.
*  @return: an SHA-512 hash string.
*/
static int sha512hex(lua_State *L) {
	size_t len;
	const char *data = luaL_checklstring(L, 1, &len);

	char digest[SHA512_DIGEST_STRING_LENGTH];
	SHA512_Data((const sha2_byte *)data, len, digest);
	lua_pushlstring(L, digest, SHA512_DIGEST_STRING_LENGTH - 1);
	return 1;
}

static int aes128ecbencrypt(lua_State *L) {
    size_t linput;
    size_t lkey;
    const char *sinput = luaL_checklstring(L, 1, &linput);
    const char *skey = luaL_checklstring(L, 2, &lkey);
    
    luaL_argcheck(L, linput == 16, 1, "length of input must be 16");
    luaL_argcheck(L, lkey == 16, 2, "length of key must be 16");
    
    char output[16];
    
    AES128_ECB_encrypt((const uint8_t *)sinput, (const uint8_t *)skey, (uint8_t *)output);
  
    lua_pushlstring(L, output, 16);
    
    return 1;
}

static int aes128ecbdecrypt(lua_State *L) {
    size_t linput;
    size_t lkey;
    const char *sinput = luaL_checklstring(L, 1, &linput);
    const char *skey = luaL_checklstring(L, 2, &lkey);
    
    luaL_argcheck(L, linput == 16, 1, "length of input must be 16");
    luaL_argcheck(L, lkey == 16, 2, "length of key must be 16");
    
    char output[16];
    
    AES128_ECB_decrypt((const uint8_t *)sinput, (const uint8_t *)skey, (uint8_t *)output);
    
    lua_pushlstring(L, output, 16);
    
    return 1;
}

static int aes128cbcencryptbuffer(lua_State *L) {
    size_t linput;
    size_t lkey;
    size_t liv;
    const char *sinput = luaL_checklstring(L, 1, &linput);
    const char *skey = luaL_checklstring(L, 2, &lkey);
    const char *siv = luaL_checklstring(L, 3, &liv);
    
    luaL_argcheck(L, linput % 16 == 0, 1, "length of input must be multiple of 16");
    luaL_argcheck(L, lkey == 16, 2, "length of key must be 16");
    luaL_argcheck(L, liv == 16, 3, "length of iv must be 16");
    
    char *newinput = (char *)malloc(linput);
    char *output = (char *)malloc(linput);
    
    memcpy(newinput, sinput, linput);

    AES128_CBC_encrypt_buffer((uint8_t *)output, (uint8_t *)newinput, (uint32_t)linput, (const uint8_t *)skey, (const uint8_t *)siv);
    
    lua_pushlstring(L, output, linput);
    
    free(newinput);
    free(output);
    
    return 1;
}

static int aes128cbcdecryptbuffer(lua_State *L) {
    size_t linput;
    size_t lkey;
    size_t liv;
    const char *sinput = luaL_checklstring(L, 1, &linput);
    const char *skey = luaL_checklstring(L, 2, &lkey);
    const char *siv = luaL_checklstring(L, 3, &liv);
    
    luaL_argcheck(L, linput % 16 == 0, 1, "length of input must be multiple of 16");
    luaL_argcheck(L, lkey == 16, 2, "length of key must be 16");
    luaL_argcheck(L, liv == 16, 3, "length of iv must be 16");
    
    char *output = (char *)malloc(linput);
    
    AES128_CBC_decrypt_buffer((uint8_t *)output, (uint8_t *)sinput, (uint32_t)linput, (const uint8_t *)skey, (const uint8_t *)siv);
    
    lua_pushlstring(L, output, linput);
    
    free(output);
    
    return 1;
}

/*
** Assumes the table is on top of the stack.
*/
static void set_info1 (lua_State *L) {
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, "sha2 0.1.0");
	lua_settable (L, -3);
}

static void set_info2 (lua_State *L) {
    lua_pushliteral (L, "_VERSION");
    lua_pushliteral (L, "aes128 0.1.0");
    lua_settable (L, -3);
}

static struct luaL_reg reg1[] = {
	{"exor", ex_or},
	{"sha256", sha256},
	{"sha256hex", sha256hex},
	{"sha384", sha384},
	{"sha384hex", sha384hex},
	{"sha512", sha512},
	{"sha512hex", sha512hex},
    {NULL, NULL}
};

static struct luaL_reg reg2[] = {
    {"aes128ecbencrypt", aes128ecbencrypt},
    {"aes128ecbdecrypt", aes128ecbdecrypt},
    {"aes128cbcencryptbuffer", aes128cbcencryptbuffer},
    {"aes128cbcdecryptbuffer", aes128cbcdecryptbuffer},
    {NULL, NULL}
};

int luaopen_sha2(lua_State *L) {
	luaL_openlib(L, "sha2", reg1, 0);
	set_info1 (L);
	return 1;
}

int luaopen_aes128(lua_State *L) {
    luaL_openlib(L, "aes128", reg2, 0);
    set_info2 (L);
    return 1;
}

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);
    
    luaopen_sha2(L);
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
    
    top = lua_gettop(L);
    luaopen_aes128(L);
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeCryptoExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeCryptoExtension(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    
    printf("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeCryptoExtension(dmExtension::AppParams* params)
{
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeCryptoExtension(dmExtension::Params* params)
{
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(Crypto, LIB_NAME, AppInitializeCryptoExtension, AppFinalizeCryptoExtension, InitializeCryptoExtension, 0, 0, FinalizeCryptoExtension)


