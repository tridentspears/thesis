"""
URL configuration for ThesisFinal project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from Website import views




urlpatterns = [
    path('admin/', admin.site.urls),
    path('', views.main, name='main'),
    path('Signup/', views.sign_up, name='Signup'),
    path('Login/', views.sign_in, name='Login'),
    path('Home/', views.home, name='Home'),
    path('AddressForm/', views.add_information, name='Add_form'),
    path('Status/',views.status, name ='Status'),
    path('Door_data/',views.door_data,name ='Door_data'),
    path('Toggle_door/',views.toggle_door,name ='Toggle_door'),
    path('PIN/<str:username>/', views.arduino_user_info, name='pin_number'),
    path('Exit/', views.sign_out, name='Exit'),
]
