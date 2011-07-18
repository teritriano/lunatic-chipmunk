//Copyright (c) 2011 Oscar (.teri) Triano <teritriano@gmail.com>

//This software is provided 'as-is', without any express or implied
//warranty. In no event will the authors be held liable for any damages
//arising from the use of this software.

//Permission is granted to anyone to use this software for any purpose,
//including commercial applications, and to alter it and redistribute it
//freely, subject to the following restrictions:

   //1. The origin of this software must not be misrepresented; you must not
   //claim that you wrote the original software. If you use this software
   //in a product, an acknowledgment in the product documentation would be
   //appreciated but is not required.

   //2. Altered source versions must be plainly marked as such, and must not be
   //misrepresented as being the original software.

   //3. This notice may not be removed or altered from any source
   //distribution.

#include <stdio.h>
#include <string.h>
#include <lunatic_chipmunk.h>

int luaopen_chipmunk(lua_State *vm){
    printf("\nInit chipmunk\n");
    luaL_Reg functions[] = {
    {"NewSpace", chipmunk_NewSpace},
    {NULL, NULL}};
    
    luaL_register(vm, "chipmunk", functions);
    lua_pop(vm, 1);
    
    luaL_Reg scenemeta[] = {
    {"__newindex", chipmunk_space_newindex},
    {"__index", chipmunk_space_index},
    {"__gc", chipmunk_space_gc},
    {NULL, NULL}};
    lua_createtable(vm, 0, 3);
    luaL_register(vm, NULL, scenemeta);
    lua_setfield(vm, LUA_REGISTRYINDEX, "chipmunk.spacemeta");
    
    luaL_Reg bodymeta[] = {
    {"__newindex", chipmunk_body_newindex},
    {"__index", chipmunk_body_index},
    {"__gc", chipmunk_body_gc},
    {NULL, NULL}};
    lua_createtable(vm, 0, 3);
    luaL_register(vm, NULL, scenemeta);
    lua_setfield(vm, LUA_REGISTRYINDEX, "chipmunk.bodymeta");
    
}

static int chipmunk_NewSpace(lua_State *vm){
    cpSpace *space = lua_newuserdata(vm, sizeof(cpSpace));
    cpSpaceInit(space);
    lua_getfield(vm, LUA_REGISTRYINDEX, "chipmunk.spacemeta");
    lua_setmetatable(vm, -2);
    return 1;
}

static int chipmunk_space_newindex(lua_State *vm){
    const char *key = lua_tostring(vm, 2);
    return 0;
}

static int chipmunk_space_index(lua_State *vm){
    const char *key = lua_tostring(vm, 2);
    if (strcmp("Step", key) == 0){
        lua_pushcfunction(vm, chipmunk_space_Step);
        return 1;
    }
    else if (strcmp("AddBody", key) == 0){
        lua_pushcfunction(vm, chipmunk_space_AddBody);
        return 1;
    }
    lua_pushnil(vm);
    return 1;
}

static int chipmunk_space_gc(lua_State *vm){
    cpSpace *space = (cpSpace *)lua_touserdata(vm, 1);
    cpSpaceDestroy(space);
    printf("Delete space: %p\n", space);
}

static int chipmunk_space_Step(lua_State *vm){
    //space, number
    cpSpace *space = (cpSpace *)lua_touserdata(vm, 1);
    cpSpaceStep(space, lua_tonumber(vm, 2));
    return 0;
}

static int chipmunk_space_AddBody(lua_State *vm){
    //space, body
    cpSpace *space = lua_touserdata(vm, 1);
    cpBody *body = lua_touserdata(vm, 2);
    cpSpaceAddBody(space, body);
    return 0;
}

static int chipmunk_NewBody(lua_State *vm){
    //mass, moi
        cpFloat m = INFINITY, moi = INFINITY;
    if (lua_isnumber(vm, 1)){
        m = lua_tonumber(vm, 1);
    }
    if (lua_isnumber(vm, 2)){
        moi = lua_tonumber(vm, 2);
    }
    cpBody *body = lua_newuserdata(vm, sizeof(cpBody));
    cpBodyInit(body, m, moi);
    lua_getfield(vm, LUA_REGISTRYINDEX, "chipmunk.bodymeta");
    lua_setmetatable(vm, -2);
    return 1;
}

static int chipmunk_body_newindex(lua_State *vm){
    const char *key = lua_tostring(vm, 2);
    cpBody *body = lua_touserdata(vm, 1);
    if (strcmp("pos", key) == 0 && lua_istable(vm, 3)){
        cpBodySetPos(body, chipmunk_TableTocpVect(3, vm));
    }
    else if (strcmp("vel", key) == 0 && lua_istable(vm, 3)){
        cpBodySetVel(body, chipmunk_TableTocpVect(3, vm));
    }
    return 0;
}

static int chipmunk_body_index(lua_State *vm){
    const char *key = lua_tostring(vm, 2);
    cpBody *body = lua_touserdata(vm, 1);
    if (strcmp("pos", key) == 0){
        cpVectToTable(cpBodyGetPos(body), vm);
        return 1;
    }
    else if (strcmp("vel", key) == 0){
        cpVectToTable(cpBodyGetVel(body), vm);
        return 1;
    }
    lua_pushnil(vm);
    return 1;
}

static int chipmunk_body_gc(lua_State *vm){
    cpBody *body = lua_touserdata(vm, 1);
    cpBodyDestroy(body);
    printf("Delete body: %p\n", body);
}
