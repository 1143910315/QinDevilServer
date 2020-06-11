#ifndef BUFFER_H
#define BUFFER_H

#include <QObject>

class Buffer : public QObject {
    Q_OBJECT
public:
    explicit Buffer(uint length, QObject *parent = nullptr);
    char *buffer;
    const uint length;
    ~Buffer();
};

#endif // BUFFER_H
