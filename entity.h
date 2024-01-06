#ifndef ENTITY
#define ENTITY

enum COMPONENTS {
    POSITION_COMPONENT,
    COMPONENTS_LENGTH
};

typedef struct {
    int x;
    int y;
} PositionComponent;

typedef struct {
    char* id;
    void* components[255];
} Entity;

#endif // ENTITY
