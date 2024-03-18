from django.shortcuts import render, redirect
from django.contrib import messages
from django.contrib.auth import login, authenticate, logout
from .forms import RegisterForm, Loginform, AddressInformation
from django.contrib.auth.decorators import login_required
from .models import AddressInfo, DoorLock,UserInfo
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.db import IntegrityError,transaction
import json
def main(request):
    return render(request, 'Main.html')
def sign_up(request):
    if request.method == 'GET':
        form = RegisterForm()
        return render(request, 'Signup.html', {'form': form})

    if request.method == 'POST':
        form = RegisterForm(request.POST)
        if form.is_valid():
            user = form.save()
            print(f'User instance: {user}')  # Print user instance
            print(f'Hashed password: {user.password}')  # Print hashed password
            messages.success(request, 'You have signed up successfully.')
            return redirect('Login')
        else:
            print(f'Form errors: {form.errors}')
            return render(request, 'Signup.html', {'form': form})


def sign_in(request):
    if request.method == 'POST':
        form = Loginform(request.POST)
        if form.is_valid():
            username = form.cleaned_data['username']
            password = form.cleaned_data['password']
            print(f'Authenticating with username: {username} and password: {password}')  # Print username and password
            user = authenticate(request, username=username, password=password)
            print(f'Authentication result: {user}')  # Print authentication result
            if user:
                login(request, user)
                messages.success(request, f'Hi {username.title()}, welcome back!')
                return redirect('Home')
        else:
            messages.error(request, 'Invalid username or password')
    else:
        form = Loginform()

    return render(request, 'Login.html', {'form': form})


def sign_out(request):
    logout(request)
    messages.success(request,'You have been logged out.')
    return redirect('main')

@login_required
def home(request):
    if request.user.is_authenticated:
        addressinfo = AddressInfo.objects.get(user=request.user)  # Use get method here
        context = {
            'username': request.user.username,
            'firstname': request.user.firstname,
            'lastname': request.user.lastname,
            'email': request.user.email,
            'phone_number': addressinfo.phone_number,
            'address': addressinfo.address
        }
    else:
        context = {'username': 'Guest'}
    if request.headers.get('X-Requested-With') == 'XMLHttpRequest':
        return render(request, 'user_info.html', context)
    else:
        return render(request, 'Home.html', context)
@login_required
def add_information(request):
    addressinfo, created = AddressInfo.objects.get_or_create(user=request.user)
    if request.method == 'POST':
        form = AddressInformation(request.POST, instance=addressinfo)
        if form.is_valid():
            form.save()
            messages.success(request, 'Your information has been updated.')
            return redirect('Home')
    else:
        form = AddressInformation(instance=addressinfo)
    return render(request, 'Address.html', {'form': form})


@login_required
def status(request):
    if request.method == 'GET':
        doors = DoorLock.objects.filter(address__user=request.user)
        context = {'doors': doors}
        return render(request, 'Status.html', context)

@csrf_exempt
def toggle_door(request):
    if request.method == 'POST':
        data = json.loads(request.body)
        door_id = data.get('door_id')
        try:
            with transaction.atomic():
                door_lock = DoorLock.objects.select_for_update().get(id=door_id, address__user=request.user)
                door_lock.Door_status = not door_lock.Door_status
                door_lock.save()
        except IntegrityError:
            return JsonResponse({'error': 'FOREIGN KEY constraint failed'}, status=400)
        return JsonResponse({'door_status': door_lock.Door_status})

def door_data(request):
    if request.method == 'GET':
        doors = DoorLock.objects.filter(address__user=request.user)
        data = {'doors': [{'id': door.id, 'status': door.Door_status, 'name': door.door_name, 'address': door.address.address} for door in doors]}
        return JsonResponse(data)

# getting pin number for arduino

def arduino_user_info(request, username):
    if request.method == 'GET':
        user = UserInfo.objects.get(username=username)
        addressinfo = AddressInfo.objects.get(user=user)
        phone = addressinfo.phone_number
        pin = phone[-4:] # taking the last 4 number of the cellphone as the pin number
        email = user.email
        return JsonResponse({'username': username, 'phone': phone, 'pin': pin,'email': email})