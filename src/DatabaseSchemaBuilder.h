#ifndef DATABASESCHEMABUILDER_H
#define DATABASESCHEMABUILDER_H

#include "ISchemaInspector.h"
#include "dbdiff/DatabaseSchemaInfoTypes.h"

DatabaseSchema buildDatabaseSchema(ISchemaInspector& inspector);

#endif // DATABASESCHEMABUILDER_H
