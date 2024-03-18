from django.contrib import admin
from .models import DoorLock, UserInfo, AddressInfo

class DoorLockAdmin(admin.ModelAdmin):
    fields = ('address', 'door_name', 'Door_status')

admin.site.register(DoorLock, DoorLockAdmin)
admin.site.register(UserInfo)
admin.site.register(AddressInfo)