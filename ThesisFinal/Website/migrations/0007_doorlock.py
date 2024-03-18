# Generated by Django 4.2.6 on 2023-12-24 08:38

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('Website', '0006_alter_addressinfo_address_and_more'),
    ]

    operations = [
        migrations.CreateModel(
            name='DoorLock',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('Door_status', models.BooleanField(default=False)),
                ('address', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to='Website.addressinfo')),
            ],
            options={
                'db_table': 'Door',
            },
        ),
    ]