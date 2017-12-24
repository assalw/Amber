#pragma once

int loadPng(const char *file, const char *name);

int getPngId(const char *name);

int getPngWidth(int id);
int getPngHeight(int id);
void * getPngData(int id);
