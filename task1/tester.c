#include "userspace_api.h"

#include <stdio.h>
#include <memory.h>
#include <errno.h>

#define EMAIL "Igor@melnikov.name"

int main() {
    struct UserPersonalData data;
    memcpy(data.name, "Igor", 5);
    memcpy(data.surname, "Igor", 5);
    memcpy(data.ph_number, "+79653668279", 13);
    memcpy(data.email, EMAIL, sizeof(EMAIL));
    data.age=20;

    add_user(&data);
    struct UserPersonalData another;

    memcpy(data.name, "Jhon", 5);
    memcpy(data.surname, "Jhon", 5);
    memcpy(data.ph_number, "+79653668279", 13);
    memcpy(data.email, "anon", sizeof("anon"));
    int ret = add_user(&data);
    if (ret) {
        return ret;
    }

    ret = get_user("Jhon", 5, &another);

    if (ret) {
        return ret;
    }


    another.email[63] = 0;

    printf("%s\n",
           another.email
           );
    return 0;
}