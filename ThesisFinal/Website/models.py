from django.db import models
from django.contrib.auth.models import AbstractBaseUser, PermissionsMixin, BaseUserManager, Group, Permission
# Create your models here.
class Usermanager(BaseUserManager):
    def create_user(self, username, firstname, lastname, email=None, password=None):
        if not username:
            raise ValueError('The Username field must be set')
        user = self.model(username=username, firstname=firstname, lastname=lastname, email=email)
        user.set_password(password)
        user.save(using=self._db)
        return user

    def create_superuser(self, username, firstname, lastname, email=None, password=None):
        user = self.create_user(username, firstname, lastname, email, password)
        user.is_staff = True
        user.is_active = True
        user.is_superuser = True
        user.save(using=self._db)
        return user

class UserInfo(AbstractBaseUser, PermissionsMixin):
    firstname = models.CharField(max_length=100)
    lastname = models.CharField(max_length=100)
    username = models.CharField(max_length=100, unique=True)
    email = models.EmailField(max_length=255)
    is_active = models.BooleanField(default=True)
    is_staff = models.BooleanField(default=False)

    groups = models.ManyToManyField(Group, related_name='user_info_set')
    user_permissions = models.ManyToManyField(Permission, related_name='user_info_set')

    USERNAME_FIELD = 'username'
    REQUIRED_FIELDS = ['firstname', 'lastname', 'email']
    objects = Usermanager()

    class Meta:
        db_table = 'user'


class AddressInfo(models.Model):
    user = models.ForeignKey(UserInfo, on_delete=models.CASCADE)
    address = models.CharField(max_length=255)
    phone_number = models.CharField(max_length=20)
    objects = models.Manager()
    class Meta:
        db_table = 'user_address_info'

    def __str__(self):
        return f'{self.address}'


class DoorLock(models.Model):
    address = models.ForeignKey(AddressInfo, on_delete=models.CASCADE)
    Door_status = models.BooleanField(default=False)
    door_name = models.CharField(max_length=255, default='front door')
    objects = models.Manager()
    class Meta:
        db_table = 'Door'

    def __str__(self):
        return self.door_name